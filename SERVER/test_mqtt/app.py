# -*- coding: utf-8 -*-
# app.py - ESP32 Dashboard + SQLite + Web DB + CSV Export
import eventlet
eventlet.monkey_patch()

from flask import Flask, render_template, jsonify, request, send_file
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt
from datetime import datetime
import threading
import sqlite3
import os
import io
import csv

app = Flask(__name__)
socketio = SocketIO(app, async_mode='eventlet', cors_allowed_origins="*")

# --- DATABASE PATH (AN TOAN, TRONG THU MUC HIEN TAI) ---
DB_PATH = '/home/ntv/iot_data/data.db'  # ÐU?NG D?N M?I, QUY?N 777
def init_db():
    try:
        # T?o thu m?c n?u c?n
        db_dir = os.path.dirname(DB_PATH)
        if db_dir and not os.path.exists(db_dir):
            os.makedirs(db_dir, exist_ok=True)
        
        conn = sqlite3.connect(DB_PATH)
        c = conn.cursor()
        c.execute('''
            CREATE TABLE IF NOT EXISTS sensor_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT,
                temp REAL,
                ph REAL,
                do_val REAL,
                motor TEXT
            )
        ''')
        c.execute('DELETE FROM sensor_data WHERE id NOT IN (SELECT id FROM sensor_data ORDER BY id DESC LIMIT 10000)')
        conn.commit()
        conn.close()
        print(f"DB created: {os.path.abspath(DB_PATH)}")
    except Exception as e:
        print(f"DB ERROR: {e}")
        print(f"Check permissions: {os.path.dirname(DB_PATH) or '.'}")

init_db()

# --- REALTIME DATA ---
MAX_POINTS = 100
chart_data = {
    'temp': [], 'ph': [], 'do': [], 'motor': 'STOPPED', 'labels': [], 'table': []
}

# --- SAVE TO DB ---
def save_to_db(temp=None, ph=None, do_val=None, motor=None):
    try:
        conn = sqlite3.connect(DB_PATH)
        c = conn.cursor()
        c.execute('''
            INSERT INTO sensor_data (timestamp, temp, ph, do_val, motor)
            VALUES (?, ?, ?, ?, ?)
        ''', (datetime.now().strftime("%Y-%m-%d %H:%M:%S"), temp, ph, do_val, motor))
        conn.commit()
        conn.close()
    except Exception as e:
        print(f"Save error: {e}")

# --- MQTT CLIENT ---
def on_connect(client, userdata, flags, rc):
    print(f"MQTT Connected: rc={rc}")
    if rc == 0:
        client.subscribe('esp32/data/#')
        print("Subscribed to esp32/data/#")

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()
    print(f"MQTT: {topic} = {payload}")

    time_str = datetime.now().strftime("%H:%M:%S")
    row = {'time': time_str, 'temp': '-', 'ph': '-', 'do': '-', 'motor': '-'}

    temp_val = ph_val = do_val = motor_val = None

    if 'temp' in topic:
        chart_data['temp'].append(float(payload))
        row['temp'] = payload
        temp_val = float(payload)
    elif 'ph' in topic:
        chart_data['ph'].append(float(payload))
        row['ph'] = payload
        ph_val = float(payload)
    elif 'do' in topic:
        chart_data['do'].append(float(payload))
        row['do'] = payload
        do_val = float(payload)
    elif 'motor' in topic:
        chart_data['motor'] = payload
        row['motor'] = payload
        motor_val = payload

    save_to_db(temp_val, ph_val, do_val, motor_val)

    # Update realtime table
    chart_data['table'].append(row)
    if len(chart_data['table']) > 20:
        chart_data['table'].pop(0)

    # Limit chart
    for key in ['temp', 'ph', 'do']:
        if len(chart_data[key]) > MAX_POINTS:
            chart_data[key].pop(0)
    if len(chart_data['labels']) >= MAX_POINTS:
        chart_data['labels'].pop(0)
    chart_data['labels'].append(time_str)

    socketio.emit('update_all', chart_data)

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect('127.0.0.1', 1883, 60)
threading.Thread(target=mqtt_client.loop_forever, daemon=True).start()

# --- ROUTES ---
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/db')
def db_page():
    page = request.args.get('page', 1, type=int)
    per_page = 50
    search = request.args.get('search', '')

    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()

    if search:
        c.execute('SELECT COUNT(*) FROM sensor_data WHERE timestamp LIKE ? OR motor LIKE ?', (f'%{search}%', f'%{search}%'))
    else:
        c.execute('SELECT COUNT(*) FROM sensor_data')
    total = c.fetchone()[0]

    offset = (page - 1) * per_page
    if search:
        c.execute('''
            SELECT timestamp, temp, ph, do_val, motor FROM sensor_data 
            WHERE timestamp LIKE ? OR motor LIKE ?
            ORDER BY id DESC LIMIT ? OFFSET ?
        ''', (f'%{search}%', f'%{search}%', per_page, offset))
    else:
        c.execute('''
            SELECT timestamp, temp, ph, do_val, motor FROM sensor_data 
            ORDER BY id DESC LIMIT ? OFFSET ?
        ''', (per_page, offset))
    rows = c.fetchall()
    conn.close()

    total_pages = (total + per_page - 1) // per_page
    return render_template('db.html', rows=rows, page=page, total_pages=total_pages, search=search, total=total)

@app.route('/export_csv')
def export_csv():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute('SELECT timestamp, temp, ph, do_val, motor FROM sensor_data ORDER BY id DESC')
    rows = c.fetchall()
    conn.close()

    output = io.StringIO()
    writer = csv.writer(output)
    writer.writerow(['Time', 'Temp', 'pH', 'DO', 'Motor'])
    writer.writerows(rows)
    output.seek(0)

    return send_file(
        io.BytesIO(output.getvalue().encode('utf-8')),
        mimetype='text/csv',
        as_attachment=True,
        download_name=f'data_{datetime.now().strftime("%Y%m%d_%H%M%S")}.csv'
    )

@socketio.on('connect')
def handle_connect():
    print("Web client connected")
    socketio.emit('update_all', chart_data)

if __name__ == '__main__':
    print("DASHBOARD: http://192.168.5.181:5000")
    print("DATABASE:  http://192.168.5.181:5000/db")
    print("EXPORT:    http://192.168.5.181:5000/export_csv")
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)