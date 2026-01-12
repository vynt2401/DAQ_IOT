import 'package:flutter/material.dart';
import 'package:shrimp_farming/models/sensor_model.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class ShrimpProvider with ChangeNotifier {
  late MqttServerClient client;

  // 1. KHAI BÁO BIẾN THỰC TẾ (KHÔNG DÙNG GET)
  // Việc dùng biến thực tế giúp lưu giữ dữ liệu khi hàm copyWith cập nhật
  List<SensorModel> sensors = [
    SensorModel(
      title: "Nhiệt độ nước",
      icon: "assets/icons/temp.png",
      backgroundColor: Colors.yellow,
      textColor: Colors.black,
      value: 0.0,
    ),
    SensorModel(
      title: "Máy sục oxi",
      icon: "assets/icons/motor.png",
      backgroundColor: Colors.blue.shade500,
      textColor: Colors.black,
      lottiePath: "assets/lottie/fan.json",
      isActive: false,
    ),
    SensorModel(
      title: "Máy cho ăn",
      icon: "assets/icons/fish.png",
      backgroundColor: Colors.orangeAccent.shade700,
      textColor: Colors.black,
      lottiePath: "assets/lottie/fish.json",
      isActive: false,
    ),
    SensorModel(
      title: "Nồng độ pH",
      icon: "assets/icons/ph.png",
      backgroundColor: Colors.lightBlueAccent,
      textColor: Colors.black,
      value: 0.0,
    ),
  ];

  ShrimpProvider() {
    setupMqtt();
  }

  Future<void> setupMqtt() async {
    // 2. Cấu hình Server: Đặt đúng
    client = MqttServerClient('192.168.137.246', 'flutter_client');
    client.port = 1883;
    client.keepAlivePeriod = 20;
    client.logging(on: true);

    // Callbacks để theo dõi trạng thái
    client.onConnected = () => print(" MQTT Connected!");
    client.onDisconnected = () => print(" MQTT Disconnected!");

    try {
      await client.connect();

      // 3. Đăng ký các Topic có trong Firmware [cite: 33, 51, 60]
      client.subscribe("esp32/data/temp", MqttQos.atMostOnce);
      client.subscribe("esp32/data/ph", MqttQos.atMostOnce);
      client.subscribe("esp32/data/motor", MqttQos.atMostOnce);
      client.subscribe("esp32/data/feed_remaining", MqttQos.atMostOnce);

      // 4. Lắng nghe dữ liệu đổ về
      client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
        final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
        final String topic = c[0].topic;
        final String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

        print("📩 Nhận: [$topic] -> $message");
        _updateSensorData(topic, message);
      });
    } catch (e) {
      print(" Connection failed: $e");
    }
  }

  void _updateSensorData(String topic, String message) {
    final cleanMessage = message.trim();
    double? newValue = double.tryParse(cleanMessage);

    switch (topic) {
      case "esp32/data/temp":
        if (newValue != null) {
          sensors[0] = sensors[0].copyWith(value: newValue); // Cập nhật Nhiệt độ
        }
        break;

      case "esp32/data/motor":
      // Firmware gửi "RUNNING - 50%" hoặc "STOPPED"
        bool isRunning = cleanMessage.contains("RUNNING");
        sensors[1] = sensors[1].copyWith(isActive: isRunning); // Điều khiển Lottie
        break;

      case "esp32/data/feed_remaining":
        if (newValue != null) {
          sensors[2] = sensors[2].copyWith(value: newValue); // Cập nhật thức ăn
        }
        break;

      case "esp32/data/ph":
        if (newValue != null) {
          sensors[3] = sensors[3].copyWith(value: newValue); // Cập nhật pH
        }
        break;
    }

    notifyListeners();
  }
}