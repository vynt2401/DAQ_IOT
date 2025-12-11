# RUN SERVER 
## Author: Nguyen The Vy - github: https://github.com/vynt2401 - Created: Thursday, 13 November 2025
### ESP32 Aquaculture Monitoring System
**Real-time IoT Dashboard for Fish/Shrimp Farming**  
**Full-stack: ESP32 → MQTT → Flask (Python) → Responsive Web Dashboard + SQLite + Security**

### Main features
- **Realtime monitoring**: Temperature, pH, Dissolved Oxygen (DO)
- **Smart aeration control** – Auto turn on/off pump when DO < 100%
- **Automatic fish feeder** with remaining feed tracking
- **Dark-mode responsive dashboard** (mobile friendly)
- **Historical data** with search, pagination, CSV export
- **SQLite database** – persistent storage (up to 10,000 records)
- **High security**: HTTPS, LAN-only binding, Basic Auth, Firewall, Honeypot trap
- **Self-recovery**: systemd service → auto restart on power loss
- **Zero maintenance** – works 24/7 in remote fish ponds

### System architecture
```
            ESP32 Sensors & Actuators (use Orange Pi -> OS: Armbian minimal server)
                  ↓ (WiFi)
            Mosquitto MQTT Broker
                  ↓
            Flask + Socket.IO (Python)
                  ↓
            Web Dashboard (Chart.js)
                  ↓
            SQLite Database + CSV Export
```

### Core tech
- **Backend**: Python Flask, Flask-SocketIO, Paho-MQTT
- **Frontend**: HTML5, CSS3, Chart.js
- **Database**: SQLite (lightweight, no installation)
- **OS**: Armbian (Orange Pi Zero 512mb)
- **Security**: HTTPS (self-signed), UFW Firewall, Basic Auth, Honeypot

### How to run


Clone this repositories
```
git clone https://github.com/vynt2401/convert_to_wsl
```
--> Point to the folder that include this repositories

### Bash Script Enviroment
### Linux 

Check Python 

```
#Linux (ubuntu)
python --version
```

```
#Windows
python --version
```

Can install python by

```
#Linux (ubuntu)
sudo apt-get install python3
```

```
#Windows --> có thể tải tại đây
https://www.python.org/downloads/
```

Install enviroment --> Virtualvenv (Linux only)
```
#Linux (ubuntu)
python3 -m pip install virtualenv 
```

Activate Enviroment Python

```
#Linux (ubuntu)
virtualenv venv_name
source venv_name/bin/activate
```

```
#Windows
python -m venv venv_name
.\venv_name\Scripts\activate
```

Install package that include at this repositories

```
#Windows
pip install -r .\requirement.txt

#Linux (ubuntu)
python -m pip install requirement.txt
```

And run this server by

```
#Windows
python app.py

#Linux (ubuntu)
python3 app.py
```
