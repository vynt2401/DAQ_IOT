#!/usr/bin/env bash
# =============================================================================
# IoT Dashboard CLI for Orange Pi Zero (Flask + MQTT + SQLite)
# Full-featured: start/stop, backup DB, firewall management, clear port
# =============================================================================

# created on January 2026
# author: Nguyen The Vy - github: https://github.com/vynt2401


# You can use this CLI to start the server

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration (edit if needed)
APP_DIR="/home/ntv/DAQ_IOT/SERVER/test_mqtt"
APP_FILE="main.py"                     # server file name (main.py or app.py)
APP_USER="ntv"
SERVICE_NAME="iot-dashboard.service"
PYTHON_BIN="/home/ntv/DAQ_IOT/SERVER/env/bin/python3"  # your venv
LOG_FILE="${APP_DIR}/server.log"
FLASK_PORT=5000
DB_FILE="/home/ntv/iot_data/data.db"
BACKUP_DIR="/home/ntv/iot_data/backups"

# Check root privileges
if [[ $EUID -ne 0 ]]; then
    echo -e "${RED}Run with sudo: sudo iotcli <command>${NC}"
    exit 1
fi

check_setup() {
    if [[ ! -d "$APP_DIR" ]]; then
        echo -e "${RED}Directory $APP_DIR does not exist${NC}"
        exit 1
    fi
    if [[ ! -f "${APP_DIR}/${APP_FILE}" ]]; then
        echo -e "${RED}File $APP_FILE not found in $APP_DIR${NC}"
        exit 1
    fi
    if [[ ! -x "$PYTHON_BIN" ]]; then
        echo -e "${RED}Python venv not found or not executable: $PYTHON_BIN${NC}"
        exit 1
    fi
}

is_running() {
    pgrep -f "${PYTHON_BIN}.*${APP_FILE}" >/dev/null
}

get_pid() {
    pgrep -f "${PYTHON_BIN}.*${APP_FILE}"
}

clear_port() {
    local port=${1:-$FLASK_PORT}
    if sudo ss -tulnp | grep -q ":${port} "; then
        echo -e "${YELLOW}Port $port is in use. Clearing...${NC}"
        sudo fuser -k "${port}"/tcp >/dev/null 2>&1 || true
        sleep 1
    fi
}

backup_database() {
    if [[ ! -f "$DB_FILE" ]]; then
        echo -e "${RED}Database file not found: $DB_FILE${NC}"
        exit 1
    fi

    mkdir -p "$BACKUP_DIR"
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    BACKUP_FILE="${BACKUP_DIR}/data_${TIMESTAMP}.db.gz"

    echo -e "${BLUE}Backing up database...${NC}"
    gzip -c "$DB_FILE" > "$BACKUP_FILE"

    if [[ -f "$BACKUP_FILE" ]]; then
        echo -e "${GREEN}Backup successful:${NC} $BACKUP_FILE"
        echo "Size: $(du -h "$BACKUP_FILE" | awk '{print $1}')"

        # Keep only last 7 backups
        find "$BACKUP_DIR" -type f -name "data_*.db.gz" | sort -r | tail -n +8 | xargs -I {} rm -f {}
        echo "Kept last 7 backups"
    else
        echo -e "${RED}Backup failed${NC}"
        exit 1
    fi
}

case "$1" in

    start)
        check_setup
        clear_port $FLASK_PORT

        if is_running; then
            CURRENT_IP=$(hostname -I | awk '{print $1}')
            echo -e "${YELLOW}Server is already running (PID: $(get_pid))${NC}"
            echo -e "${GREEN}Dashboard: http://${CURRENT_IP}:${FLASK_PORT}${NC}"
            exit 0
        fi

        echo -e "${BLUE}Starting IoT Dashboard...${NC}"
        cd "$APP_DIR" || exit 1

        nohup "$PYTHON_BIN" "$APP_FILE" > "$LOG_FILE" 2>&1 &

        sleep 3
        if is_running; then
            CURRENT_IP=$(hostname -I | awk '{print $1}')
            echo -e "${GREEN}Started successfully (PID: $(get_pid))${NC}"
            echo -e "${GREEN}Dashboard: http://${CURRENT_IP}:${FLASK_PORT}${NC}"
            echo ""
            echo "Log: $LOG_FILE (use 'iotcli log' to view)"
            tail -n 5 "$LOG_FILE"
        else
            echo -e "${RED}Start failed. Check log:${NC}"
            tail -n 20 "$LOG_FILE"
            exit 1
        fi
        ;;

    stop)
        if ! is_running; then
            echo -e "${YELLOW}Server is not running${NC}"
            exit 0
        fi

        PID=$(get_pid)
        echo -e "${BLUE}Stopping server (PID $PID)...${NC}"
        kill "$PID" || true
        sleep 2

        if is_running; then
            echo -e "${YELLOW}Force killing...${NC}"
            kill -9 "$PID"
        fi

        echo -e "${GREEN}Server stopped${NC}"
        ;;

    restart)
        "$0" stop
        sleep 2
        "$0" start
        ;;

    status)
        echo -e "${BLUE}=== IoT Dashboard Status ===${NC}"
        CURRENT_IP=$(hostname -I | awk '{print $1}')
        if is_running; then
            PID=$(get_pid)
            echo -e "${GREEN}RUNNING${NC} - PID: $PID"
            ps -p "$PID" -o pid,start_time,cmd --no-headers
            echo -e "${GREEN}Dashboard: http://${CURRENT_IP}:${FLASK_PORT}${NC}"
        else
            echo -e "${RED}STOPPED${NC}"
        fi
        echo ""
        echo "Current IP: $CURRENT_IP"
        echo "RAM usage:"
        free -h | head -n 2
        echo ""
        tail -n 10 "$LOG_FILE" 2>/dev/null || echo "No log yet"
        ;;

    log)
        tail -n 40 "$LOG_FILE" 2>/dev/null || echo -e "${YELLOW}No log file found${NC}"
        ;;

    backup)
        backup_database
        ;;

    clear-port)
        PORT=${2:-$FLASK_PORT}
        echo -e "${BLUE}Clearing port $PORT...${NC}"
        sudo fuser -k "${PORT}"/tcp >/dev/null 2>&1 || true
        echo -e "${GREEN}Port $PORT cleared${NC}"
        ;;

    firewall)
        if ! command -v ufw >/dev/null; then
            echo -e "${RED}UFW not installed. Install with: sudo apt install ufw${NC}"
            exit 1
        fi

        case "$2" in
            status)
                sudo ufw status verbose
                ;;
            enable)
                sudo ufw --force enable
                echo -e "${GREEN}UFW enabled${NC}"
                ;;
            disable)
                sudo ufw --force disable
                echo -e "${YELLOW}UFW disabled${NC}"
                ;;
            allow-port)
                [ -z "$3" ] && { echo "Usage: sudo iotcli firewall allow-port <port>"; exit 1; }
                sudo ufw allow "$3"/tcp
                echo -e "${GREEN}Allowed port $3/tcp${NC}"
                ;;
            deny-port)
                [ -z "$3" ] && { echo "Usage: sudo iotcli firewall deny-port <port>"; exit 1; }
                sudo ufw deny "$3"/tcp
                echo -e "${YELLOW}Denied port $3/tcp${NC}"
                ;;
            *)
                echo -e "${BLUE}Firewall commands:${NC}"
                echo "  status          Show UFW status"
                echo "  enable          Enable firewall"
                echo "  disable         Disable firewall"
                echo "  allow-port <port>  Allow a port (tcp)"
                echo "  deny-port <port>   Deny a port (tcp)"
                echo ""
                echo "Example:"
                echo "  sudo iotcli firewall status"
                echo "  sudo iotcli firewall allow-port 5000"
                ;;
        esac
        ;;

    enable-service)
        echo -e "${BLUE}Creating systemd service...${NC}"
        cat > "/etc/systemd/system/$SERVICE_NAME" << EOF
[Unit]
Description=IoT Dashboard Flask + MQTT + SQLite
After=network.target mosquitto.service

[Service]
User=$APP_USER
WorkingDirectory=$APP_DIR
ExecStart=$PYTHON_BIN $APP_DIR/$APP_FILE
Restart=always
RestartSec=10
StandardOutput=append:$LOG_FILE
StandardError=append:$LOG_FILE

[Install]
WantedBy=multi-user.target
EOF
        systemctl daemon-reload
        systemctl enable "$SERVICE_NAME"
        echo -e "${GREEN}Service $SERVICE_NAME enabled${NC}"
        echo "Start it: sudo systemctl start $SERVICE_NAME"
        ;;

    disable-service)
        systemctl disable "$SERVICE_NAME" 2>/dev/null || true
        systemctl stop "$SERVICE_NAME" 2>/dev/null || true
        echo -e "${YELLOW}Service disabled${NC}"
        ;;

    *)
        echo -e "${BLUE}IoT Dashboard CLI${NC}"
        echo "Usage: sudo iotcli <command>"
        echo ""
        echo -e "${YELLOW}Available commands:${NC}"
        echo "  start           Start the server"
        echo "  stop            Stop the server"
        echo "  restart         Restart the server"
        echo "  status          Show status and IP"
        echo "  log             Show recent logs"
        echo "  backup          Backup SQLite database"
        echo "  clear-port [port] Clear occupied port (default 5000)"
        echo "  firewall        Manage UFW firewall (status, enable, disable, allow-port <port>, deny-port <port>)"
        echo "  enable-service  Enable auto-start on boot"
        echo "  disable-service Disable auto-start"
        echo ""
        echo -e "${GREEN}Examples:${NC}"
        echo "  sudo iotcli start"
        echo "  sudo iotcli backup"
        echo "  sudo iotcli firewall allow-port 5000"
        echo "  sudo iotcli status"
        exit 1
        ;;
esac