# SERVER 

# ESP32 Aquaculture Monitoring System
**Real-time IoT Dashboard for Fish/Shrimp Farming**  
**Full-stack: ESP32 → MQTT → Flask (Python) → Responsive Web Dashboard + SQLite + Security**



### Tính năng chính
- **Realtime monitoring**: Temperature, pH, Dissolved Oxygen (DO)
- **Smart aeration control** – Auto turn on/off pump when DO < 100%
- **Automatic fish feeder** with remaining feed tracking
- **Dark-mode responsive dashboard** (mobile friendly)
- **Historical data** with search, pagination, CSV export
- **SQLite database** – persistent storage (up to 10,000 records)
- **High security**: HTTPS, LAN-only binding, Basic Auth, Firewall, Honeypot trap
- **Self-recovery**: systemd service → auto restart on power loss
- **Zero maintenance** – works 24/7 in remote fish ponds

### Kiến trúc hệ thống
=
ESP32 Sensors & Actuators
      ↓ (WiFi)
Mosquitto MQTT Broker
      ↓
Flask + Socket.IO (Python)
      ↓
Web Dashboard (Chart.js)
      ↓
SQLite Database + CSV Export


### Công nghệ sử dụng
- **Backend**: Python Flask, Flask-SocketIO, Paho-MQTT
- **Frontend**: HTML5, CSS3, Chart.js
- **Database**: SQLite (lightweight, no installation)
- **OS**: Armbian (Orange Pi Zero 512mb)
- **Security**: HTTPS (self-signed), UFW Firewall, Basic Auth, Honeypot

### Cách chạy (triển khai thực tế)

```bash
# 1. Clone repo
git clone https://github.com/yourusername/esp32-aquaculture.git
cd esp32-aquaculture

# 2. Tạo venv & cài thư viện
python3 -m venv venv
source venv/bin/activate
pip install flask flask-socketio eventlet paho-mqtt

# 3. Chạy (hoặc dùng systemd service đã cung cấp)
python3 app.py

