
// --- BAO GỒM TẤT CẢ THƯ VIỆN ---
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_MAX31865.h>

 //--- CẤU HÌNH WI-FI & MQTT ---
const char* ssid = "utp";
const char* password = "12345678";
const char* mqtt_server = "192.168.137.14";
const int mqtt_port = 1883;

 WiFiClient espClient;
 PubSubClient client(espClient);

// --- CẤU HÌNH CẢM BIẾN NHIỆT ĐỘ (PT100) ---
#define MAX_CS 5
const float RREF = 430.0;
const float RNOMINAL = 100.0;
const float SAI_SO_OFFSET = 1.6;
Adafruit_MAX31865 thermo = Adafruit_MAX31865(MAX_CS);

// --- CẤU HÌNH CHÂN L298N (MOTOR) ---
const int IN1_PIN = 26;
const int IN2_PIN = 25;
const int ENA_PIN = 27;
const int MOTOR_SPEED_50_PERCENT = 127;
// --- Motor Feeder
const int IN3_PIN = 13;
const int IN4_PIN = 14;
const int ENB_PIN = 33;
const int MOTOR_FEED_SPEED = 127;

// --- MÁY CHO ĂN ---
#define BUZZER_PIN 21
#define FEED_INTERVAL 10000    // 10 giây
#define FEED_AMOUNT 10.0       // Trừ 10kg/lần
#define REFILL_INTERVAL 30000  // Sau 30 giây nạp lại

float foodWeight = 50.0;
unsigned long lastFeedTime = 0;
unsigned long emptyTime = 0;   // thời điểm hết thức ăn

// --- BIẾN TOÀN CỤC ---
float currentTemp = 0.0;
float currentPH = 7.5;
float currentDO = 100;
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
//// Motor feeder
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
// HÀM BUZZER + CHO ĂN 
// ============================================================
void beep(int duration_ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration_ms);
  digitalWrite(BUZZER_PIN, LOW);
}

void handleFeeding() {
  unsigned long now = millis();

  // Cho ăn mỗi 10 giây nếu còn thức ăn
  if (now - lastFeedTime >= FEED_INTERVAL && foodWeight > 0) {
    lastFeedTime = now;

    Serial.println("\n🐟 Cho ăn...");
    moveForward2();
    setSpeed2(MOTOR_FEED_SPEED);
    beep(2000);  // Bíp 1.5s
    stopMotor2();
    setSpeed2(0);
    foodWeight -= FEED_AMOUNT;
    if (foodWeight < 0) foodWeight = 0;

    Serial.print("📉 Còn lại: ");
    Serial.print(foodWeight);
    Serial.println(" kg");

    // // Gửi trạng thái thức ăn lên MQTT
     if (client.connected()) {
       client.publish("esp32/data/feed_weight", String(foodWeight, 1).c_str());
   }

    if (foodWeight == 0) {
      Serial.println("⚠️ ĐÃ HẾT THỨC ĂN!");
      beep(1500);
      beep(1500);
      emptyTime = now;
    }
  }

  // Sau 30 giây thì nạp lại 50kg
  if (foodWeight == 0 && (now - emptyTime >= REFILL_INTERVAL) && emptyTime != 0) {
    foodWeight = 50.0;
    Serial.println("\n🔄 ĐÃ NẠP LẠI 50KG THỨC ĂN!");
    beep(1500);
    Serial.println("🟢 Trạng thái: ĐANG ĐẦY");
    emptyTime = 0;

     if (client.connected()) {
       client.publish("esp32/data/feed_weight", "50.0");
       client.publish("esp32/data/feed_status", "REFILLED");
     }
  }
}

// ============================================================
// MQTT, WiFi, và Sensor cũ
// ============================================================
 void reconnect() {
   while (!client.connected()) {
     Serial.print("Dang thu ket noi MQTT den ");
     Serial.print(mqtt_server);
     Serial.print("...");

     if (client.connect("ESP32_Dashboard_Client")) {
       Serial.println("Da ket noi!");
     } else {
       Serial.print("That bai, loi = ");
       Serial.println(client.state());
       delay(5000);
     }
   }
}

float readCalibratedTemp() {
  uint8_t fault = thermo.readFault();
  if (fault) {
    thermo.clearFault();
    return -999.0;
  }
  float tempC_tho = thermo.temperature(RNOMINAL, RREF);
  return tempC_tho - SAI_SO_OFFSET;
}

float generateRandomPH() {
  return (float)random(750, 851) / 100.0;
}

float generateRandomDO() {
  return (float)random(90, 95);
}

void checkMotorLogic() {
  if (currentDO < 100.0) {
    if (motorStatus != "RUNNING") {
      Serial.println("DO < 100%. Dang bat motor suc khi...");
      moveForward();
      setSpeed(MOTOR_SPEED_50_PERCENT);
      motorStatus = "RUNNING";

       if (client.connected()) {
         client.publish("esp32/data/motor", "RUNNING - 50%");
       }
    }
  } else {
    if (motorStatus != "STOPPED") {
      Serial.println("DO da dat 100%. Dung motor.");
      stopMotor();
      setSpeed(0);
      motorStatus = "STOPPED";

       if (client.connected()) {
         client.publish("esp32/data/motor", "STOPPED");
       }
    }
  }
}

 void setup_wifi() {
   Serial.println("\n--- Khoi dong ---");
   Serial.print("Dang ket noi WiFi: ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   Serial.println("\nDa ket noi WiFi!");
   Serial.print("IP: ");
   Serial.println(WiFi.localIP());
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

  // --- Khởi tạo giá trị ban đầu ---
  currentTemp = readCalibratedTemp();
  currentPH = generateRandomPH();
  currentDO = generateRandomDO();

  // --- Gán mốc thời gian ban đầu để tránh random sớm ---
  previousMillisTemp = millis();
  previousMillisPH_DO_Generate = millis();
  previousMillisDO_Recharge = millis();

  Serial.println("\n=== KHỞI ĐỘNG MÁY CHO ĂN ===");
  Serial.println("📦 Lượng thức ăn ban đầu: 50 kg");
  Serial.println("🟢 Trạng thái: ĐANG ĐẦY");

  // ============================================================
  // 🟢 THÊM PHẦN KẾT NỐI WI-FI & MQTT ỔN ĐỊNH
  // ============================================================

  Serial.println("\n🔌 Đang kết nối Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Đã kết nối Wi-Fi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // --- Cấu hình MQTT ---
  client.setServer(mqtt_server, mqtt_port);
  Serial.print("📡 Đang thử kết nối MQTT đến ");
  Serial.println(mqtt_server);

  // --- Kết nối MQTT ổn định, không crash ---
  while (!client.connected()) {
    Serial.print("⏳ Kết nối MQTT...");
    if (client.connect("ESP32_Dashboard_Client")) {
      Serial.println("✅ Thành công!");
    } else {
      Serial.print("❌ Lỗi, mã lỗi = ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  Serial.println("📶 MQTT đã sẵn sàng!\n");
}



// ============================================================
// LOOP
// ============================================================
void loop() {
   if (!client.connected()) reconnect();
   client.loop();

  unsigned long currentMillis = millis();

  // 1️⃣ Gửi dữ liệu sensor mỗi 1s
  if (currentMillis - previousMillisTemp >= intervalTemp) {
    previousMillisTemp = currentMillis;
    float newTemp = readCalibratedTemp();
    if (newTemp != -999.0) currentTemp = newTemp;

     if (client.connected()) {
       client.publish("esp32/data/temp", String(currentTemp, 2).c_str());
       client.publish("esp32/data/ph", String(currentPH, 2).c_str());
       client.publish("esp32/data/do", String(currentDO, 1).c_str());
     }

    Serial.print("Temp: "); Serial.print(currentTemp, 2);
    Serial.print(" | pH: "); Serial.print(currentPH, 2);
    Serial.print(" | DO: "); Serial.print(currentDO, 1);
    Serial.print(" | Motor: "); Serial.println(motorStatus);
  }

  // 2️⃣ Sinh giá trị pH, DO mới mỗi 30s
  if (currentMillis - previousMillisPH_DO_Generate >= intervalPH_DO_Generate) {
    previousMillisPH_DO_Generate = currentMillis;
    currentPH = generateRandomPH();
    currentDO = generateRandomDO();
    Serial.println("\n*** (30s) Tao gia tri moi ***");
  }

  // 3️⃣ Tăng DO 0.6 mỗi 1s nếu <100
  if (currentMillis - previousMillisDO_Recharge >= intervalDO_Recharge) {
    previousMillisDO_Recharge = currentMillis;
    if (currentDO < 100.0) {
      currentDO += 0.6;
      if (currentDO > 100.0) currentDO = 100.0;
    }
  }

  // 4️⃣ Kiểm tra logic motor
  checkMotorLogic();

  // 5️⃣ Máy cho ăn tự động
  handleFeeding();

  // 6️⃣ Gửi dữ liệu số lượng thức ăn còn lại mỗi 1s
  static unsigned long previousMillisFeed = 0;
  const long intervalFeed = 1000; // 1 giây
  if (currentMillis - previousMillisFeed >= intervalFeed) {
    previousMillisFeed = currentMillis;

    if (client.connected()) {
      client.publish("esp32/data/feed_remaining", String(foodWeight, 1).c_str());
    }
  }
}
