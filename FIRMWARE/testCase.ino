#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_MAX31865.h>

// --- CẤU HÌNH WI-FI & MQTT ---
const char* ssid = "utp";
const char* password = "12345678";
const char* mqtt_server = "192.168.137.103";  // <-- THAY BẰNG IP THẬT CỦA ORANGE PI (hostname -I)
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

// --- CẢM BIẾN PT100 ---
#define MAX_CS 5
const float RREF = 430.0;
const float RNOMINAL = 100.0;
const float SAI_SO_OFFSET = 1.6;
Adafruit_MAX31865 thermo = Adafruit_MAX31865(MAX_CS);

// --- CHÂN L298N MOTOR ---
const int IN1_PIN = 26;
const int IN2_PIN = 25;
const int ENA_PIN = 27;
const int MOTOR_SPEED_50_PERCENT = 127;

// --- MOTOR FEEDER ---
const int IN3_PIN = 13;
const int IN4_PIN = 14;
const int ENB_PIN = 33;
const int MOTOR_FEED_SPEED = 127;

// --- BUZZER & CHO ĂN ---
#define BUZZER_PIN 21
#define FEED_INTERVAL 10000
#define FEED_AMOUNT 10.0
#define REFILL_INTERVAL 30000
float foodWeight = 50.0;
unsigned long lastFeedTime = 0;
unsigned long emptyTime = 0;

// --- BIẾN TOÀN CỤC ---
float currentTemp = 0.0;
float currentPH = 7.5;
float currentDO = 100.0;
String motorStatus = "STOPPED";
unsigned long previousMillisTemp = 0;
unsigned long previousMillisPH_DO_Generate = 0;
unsigned long previousMillisDO_Recharge = 0;
const long intervalTemp = 1000;
const long intervalPH_DO_Generate = 40000;
const long intervalDO_Recharge = 1000;

// ============================================================
// HÀM ĐIỀU KHIỂN MOTOR
// ============================================================
void setSpeed(int speed) {
  speed = constrain(speed, 0, 255);
  analogWrite(ENA_PIN, speed);
}
void moveForward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}
void stopMotor() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void setSpeed2(int speed) {
  speed = constrain(speed, 0, 255);
  analogWrite(ENB_PIN, speed);
}
void moveForward2() {
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}
void stopMotor2() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

// ============================================================
// BUZZER + CHO ĂN
// ============================================================
void beep(int duration_ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration_ms);
  digitalWrite(BUZZER_PIN, LOW);
}

void handleFeeding() {
  unsigned long now = millis();
  if (now - lastFeedTime >= FEED_INTERVAL && foodWeight > 0) {
    lastFeedTime = now;
    Serial.println("\n🐟 Cho ăn...");
    moveForward2();
    setSpeed2(MOTOR_FEED_SPEED);
    beep(2000);
    stopMotor2();
    setSpeed2(0);
    foodWeight -= FEED_AMOUNT;
    if (foodWeight < 0) foodWeight = 0;
    Serial.print("📉 Còn lại: "); Serial.print(foodWeight); Serial.println(" kg");
    publishIfConnected("esp32/data/feed_weight", String(foodWeight, 1).c_str());
    if (foodWeight == 0) {
      Serial.println("⚠️ ĐÃ HẾT THỨC ĂN!");
      beep(1500); beep(1500);
      emptyTime = now;
    }
  }
  if (foodWeight == 0 && (now - emptyTime >= REFILL_INTERVAL) && emptyTime != 0) {
    foodWeight = 50.0;
    Serial.println("\n🔄 ĐÃ NẠP LẠI 50KG THỨC ĂN!");
    beep(1500);
    Serial.println("🟢 Trạng thái: ĐANG ĐẦY");
    emptyTime = 0;
    publishIfConnected("esp32/data/feed_weight", "50.0");
    publishIfConnected("esp32/data/feed_status", "REFILLED");
  }
}

// ============================================================
// MQTT HELPER
// ============================================================
bool isConnected() {
  return client.connected();
}

void publishIfConnected(const char* topic, const char* payload) {
  if (isConnected()) {
    client.publish(topic, payload);
  } else {
    Serial.print("MQTT not connected, skip publish: "); Serial.println(topic);
  }
}

void reconnect() {
  int retryCount = 0;
  while (!client.connected() && retryCount < 10) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("esp32/data/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    retryCount++;
  }
}

// ============================================================
// SENSOR & LOGIC
// ============================================================
float readCalibratedTemp() {
  uint8_t fault = thermo.readFault();
  if (fault) {
    thermo.clearFault();
    return -999.0;
  }
  float tempC = thermo.temperature(RNOMINAL, RREF);
  return tempC - SAI_SO_OFFSET;
}

float generateRandomPH() {
  return (float)random(650, 851) / 100.0;
}

float generateRandomDO() {
  return (float)random(90, 95);
}

void checkMotorLogic() {
  if (currentDO < 100.0) {
    if (motorStatus != "RUNNING") {
      Serial.println("DO < 100%. Starting motor...");
      moveForward();
      setSpeed(MOTOR_SPEED_50_PERCENT);
      motorStatus = "RUNNING";
      publishIfConnected("esp32/data/motor", "RUNNING - 50%");
    }
  } else {
    if (motorStatus != "STOPPED") {
      Serial.println("DO reached 100%. Stopping motor.");
      stopMotor();
      setSpeed(0);
      motorStatus = "STOPPED";
      publishIfConnected("esp32/data/motor", "STOPPED");
    }
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  stopMotor();
  stopMotor2();
  setSpeed(0);
  setSpeed2(0);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  thermo.begin(MAX31865_2WIRE);
  randomSeed(analogRead(34));

  // Initial values
  currentTemp = readCalibratedTemp();
  currentPH = generateRandomPH();
  currentDO = generateRandomDO();

  previousMillisTemp = millis();
  previousMillisPH_DO_Generate = millis();
  previousMillisDO_Recharge = millis();

  Serial.println("\n=== ESP32 FEEDER STARTUP ===");
  Serial.println("Initial food: 50 kg");
  Serial.println("Status: FULL");

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  // Send sensor data every 1s
  if (currentMillis - previousMillisTemp >= intervalTemp) {
    previousMillisTemp = currentMillis;
    float newTemp = readCalibratedTemp();
    if (newTemp != -999.0) currentTemp = newTemp;

    publishIfConnected("esp32/data/temp", String(currentTemp, 2).c_str());
    publishIfConnected("esp32/data/ph", String(currentPH, 2).c_str());
    publishIfConnected("esp32/data/do", String(currentDO, 1).c_str());

    Serial.print("Temp: "); Serial.print(currentTemp, 2);
    Serial.print(" | pH: "); Serial.print(currentPH, 2);
    Serial.print(" | DO: "); Serial.print(currentDO, 1);
    Serial.print(" | Motor: "); Serial.println(motorStatus);
  }

  // Generate random pH/DO every 40s
  if (currentMillis - previousMillisPH_DO_Generate >= intervalPH_DO_Generate) {
    previousMillisPH_DO_Generate = currentMillis;
    currentPH = generateRandomPH();
    currentDO = generateRandomDO();
    Serial.println("\n*** (40s) Generated new pH/DO values ***");
  }

  // Recharge DO
  if (currentMillis - previousMillisDO_Recharge >= intervalDO_Recharge) {
    previousMillisDO_Recharge = currentMillis;
    if (currentDO < 100.0) {
      currentDO += 0.6;
      if (currentDO > 100.0) currentDO = 100.0;
    }
  }

  // Motor logic
  checkMotorLogic();

  // Feeding
  handleFeeding();

  // Send feed remaining every 1s
  static unsigned long previousMillisFeed = 0;
  const long intervalFeed = 1000;
  if (currentMillis - previousMillisFeed >= intervalFeed) {
    previousMillisFeed = currentMillis;
    publishIfConnected("esp32/data/feed_remaining", String(foodWeight, 1).c_str());
  }
}
