
## Full-stack system: ESP32 → MQTT → Flask (Python) → Responsive Web Dashboard + SQLite + Security

## Backend: Python Flask, Flask-SocketIO, Paho-MQTT

## Frontend: HTML5, CSS3

## atabase: SQLite (lightweight, no installation)

## OS: Armbian (Orange Pi Zero 512mb)

## Security: UFW Firewall, Basic Auth, Fail2ban


1. Clone Repository
```
bash
git clone https://github.com/vynt2401/DAQ_IOT.git
cd DAQ_IOT/SERVER
```

2. Prerequisites Check
```
Linux (Ubuntu/Armbian)
bash
python3 --version
# If not installed:
sudo apt-get update
sudo apt-get install python3 python3-pip -y
Windows
bash
python --version
# Download from: https://www.python.org/downloads/
```
3. Setup Virtual Environment
```
Linux (Ubuntu/Armbian)
bash
python3 -m pip install virtualenv
python3 -m venv env
source env/bin/activate
Windows
bash
python -m venv env
.\env\Scripts\activate
```
4. Install Dependencies
```
bash
pip install -r requirement.txt
```
Automated CLI Tool (Bash Script)
The system includes an automated CLI tool iotcli for easy server management.

Installation & Setup
Copy the script to system path:
```
bash
# Copy the bash script to /usr/local/bin
sudo cp iotcli.sh /usr/local/bin/iotcli
sudo chmod +x /usr/local/bin/iotcli
```
Configure paths in the script (if needed):
Edit the following variables in the script:
```
bash
APP_DIR="/home/ntv/DAQ_IOT/SERVER/test_mqtt"  # Your server directory
APP_USER="ntv"                                 # Your username
PYTHON_BIN="/home/ntv/DAQ_IOT/SERVER/env/bin/python3"  # Python venv path
```
CLI Usage Examples
Basic Server Control:
```
bash
# Start the server
sudo iotcli start

# Stop the server
sudo iotcli stop

# Restart the server
sudo iotcli restart

# Check server status
sudo iotcli status
Database Management:

bash
# Backup SQLite database (keeps last 7 backups)
sudo iotcli backup
Firewall Management:

bash
# Check firewall status
sudo iotcli firewall status

# Enable firewall
sudo iotcli firewall enable

# Allow Flask port (default 5000)
sudo iotcli firewall allow-port 5000

# Deny a specific port
sudo iotcli firewall deny-port 8080
Logs & Monitoring:

bash
# View server logs
sudo iotcli log

# Clear occupied port
sudo iotcli clear-port 5000
Auto-start on Boot:

bash
# Enable auto-start service
sudo iotcli enable-service

# Disable auto-start
sudo iotcli disable-service
Help & All Commands:

bash
sudo iotcli
```
## Manual Server Execution

Without CLI Tool
Linux (Ubuntu/Armbian)
```
bash
cd /path/to/SERVER/test_mqtt
source ../env/bin/activate
python3 main.py
Windows
bash
cd \path\to\SERVER\test_mqtt
..\env\Scripts\activate
python app.py
```
## Systemd Service Setup (Optional)
For production deployment with auto-restart:

Enable systemd service:
```
bash
sudo iotcli enable-service
```

Manage the service:
```
bash
# Start service
sudo systemctl start iot-dashboard.service

# Stop service
sudo systemctl stop iot-dashboard.service

# Check status
sudo systemctl status iot-dashboard.service

# View logs
sudo journalctl -u iot-dashboard.service -f
```
The service will:

Auto-start on boot

Restart automatically if crashed

Run as your specified user

Log to server.log

## Access Dashboard
After starting the server:
```
bash
# Get your Orange Pi IP address
hostname -I
```

# Access dashboard in browser
http://<YOUR_ORANGE_PI_IP>:5000
Example output from iotcli status:

text
=== IoT Dashboard Status ===
RUNNING - PID: 1234
Dashboard: http://192.168.1.100:5000

Current IP: 192.168.1.100
RAM usage:
              total        used        free
Mem:          985MB        245MB       740MB


## Security Configuration
1. Firewall Setup
```bash
# Enable firewall
sudo iotcli firewall enable

# Allow SSH (if needed)
sudo ufw allow 22/tcp

# Allow Flask dashboard port
sudo iotcli firewall allow-port 5000

# Check status
sudo iotcli firewall status
```
2. Change Default Credentials
Edit app.py or main.py to update:

Basic Auth username/password

MQTT credentials

Secret keys

## Database Management
Backup Automation
The CLI tool automatically:

Creates timestamped backups

Compresses with gzip

Keeps last 7 backups

Stores in /home/ntv/iot_data/backups/

Manual backup:
```
bash
sudo iotcli backup
```
Export Data
Access the dashboard and use:

CSV export from web interface

Direct SQLite access: /home/ntv/iot_data/data.db


# SOURCE 
GitHub: https://github.com/vynt2401

Repository: https://github.com/vynt2401/DAQ_IOT

Created: Thursday, 13 November 2025

Last Updated: January 2026

Useful Commands Cheat Sheet
```
bash
# Quick start
sudo iotcli start && sudo iotcli firewall allow-port 5000

# Full setup (one-time)
sudo iotcli enable-service
sudo iotcli firewall enable
sudo iotcli firewall allow-port 5000
sudo systemctl start iot-dashboard.service

# Maintenance
sudo iotcli backup          # Backup database
sudo iotcli status          # Check status
sudo iotcli log            # View logs
```
