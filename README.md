# DAQ_IoT – Hệ thống thu thập dữ liệu IoT đa kênh

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Python](https://img.shields.io/badge/Python-3.9%2B-blue)](https://www.python.org/)
[![Platform](https://img.shields.io/badge/Platform-OrangePi%20%7C%20Windows-green)](#)

Hệ thống thu thập và giám sát dữ liệu thời gian thực từ cảm biến công nghiệp/nghiên cứu, chạy trên Orange Pi (Armbian) kết hợp MQTT + Web Dashboard.

## Tổng quan kiến trúc


```
Cảm biến/Analog/Digital
        ↓ (ADC / UART / GPIO)
Orange Pi Zero 2W / PC (Firmware C/Python)
        ↓ (MQTT)
Mosquitto Broker (1883)
        ↓
Python Server (Flask + Socket.IO + Paho-MQTT)
        ↓
React/Vite Dashboard (Web realtime)
```

# Cấu trúc thư mục


```
DAQ_IOT/
├── firmware/               # Code nhúng chạy trên Orange Pi hoặc ESP32
├── server/                 # Backend Python (Flask + MQTT + API)
├── web/                    # Frontend React + Vite + Chart.js realtime
├── docs/                   # Tài liệu, sơ đồ mạch, ảnh thực tế
├── scripts/                # Script deploy, backup DB, v.v.
└── README.md
```



## Tính năng chính

- Thu thập dữ liệu analog 12–16 bit (MCP342x, ADS1115…)
- Hỗ trợ UART, I2C, SPI, GPIO
- Gửi dữ liệu qua MQTT (QoS 1)
- Web dashboard realtime (Chart.js + Socket.IO)
- Lưu trữ SQLite hoặc InfluxDB (tuỳ chọn)
- Responsive trên cả mobile

## Yêu cầu phần cứng

- Orange Pi Zero 2W / Orange Pi 5 / PC bất kỳ
- Mosquitto broker (có thể chạy cùng máy hoặc riêng)
- Các module ADC (ADS1115, MCP3428, v.v.)

  ## Lưu ý quan trọng khi “đổi trạng thái đi” (Orange Pi ↔ Laptop/PC Windows)

| Trường hợp                        | MQTT Broker phải để là gì?               | Ghi chú quan trọng                                                                 |
|-----------------------------------|----------------------------------------|-------------------------------------------------------------------------------------|
| Chạy toàn bộ trên Orange Pi       | `127.0.0.1` hoặc `localhost`           | Mosquitto phải chạy trên chính con Pi                                               |
| Chạy server/web trên laptop/PC    | IP thực của Orange Pi (ví dụ `192.168.1.69`)| Laptop và Pi phải cùng mạng WiFi/LAN                                               |
| Chỉ test code trên laptop (không có Pi) | `broker.hivemq.com` hoặc `test.mosquitto.org` | Dùng tạm, dữ liệu công khai                                                        |
| Chạy Mosquitto trên Windows       | `127.0.0.1`                            | Cài Mosquitto Windows hoặc dùng Docker                                             |

**Checklist nhanh trước khi chạy lại sau khi đổi máy:**

```
# 1. Kiểm tra IP Orange Pi
ip a                    # trên Pi
ipconfig                # trên Windows

# 2. Kiểm tra Mosquitto có sống không
systemctl status mosquitto          # Pi
net start mosquitto                 # Windows (nếu cài service)

# 3. Test kết nối port 1883 từ laptop
Test-NetConnection 192.168.1.69 -Port 1883    # PowerShell
telnet 192.168.1.69 1883                      # CMD (nếu có telnet)

# 4. Dùng file .env (khuyên làm luôn luôn)
MQTT_BROKER=192.168.1.69
MQTT_PORT=1883
```

## Cài đặt nhanh (trên Orange Pi (Armbian)

```
# 1. Clone repo
git clone https://github.com/vynt2401/DAQ_IOT.git
cd DAQ_IOT

# 2. Cài Mosquitto
sudo apt update && sudo apt install mosquitto mosquitto-clients -y
sudo systemctl enable mosquitto

# 3. Cài Python dependencies (server)
cd server
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# 4. Chạy server
python app.py
# → Mặc định chạy trên http://0.0.0.0:5000 và kết nối MQTT localhost
```

## Chạy trên Windows(dev/test)

```
# Server
cd server
venv\Scripts\activate
set MQTT_BROKER=192.168.x.x     # IP của Orange Pi (nếu broker)
python app.py

# Web
cd web
npm install
npm run dev
# → Mở http://localhost:5173
```

# NOTE

### Đổi broker nhanh
Tạo file .envi trong thư mục server/:
```
MQTT_BROKER=192.168.1.69
MQTT_PORT=1883
MQTT_USER=admin
MQTT_PASS=123456
```



### Contributes

Rất hoan nghênh pull request!

Thêm driver cảm biến mới → thư mục firmware/drivers/
Cải thiện giao diện → thư mục web/
Thêm InfluxDB + Grafana → nhánh influxdb

