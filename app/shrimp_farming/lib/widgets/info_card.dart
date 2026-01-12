import 'package:flutter/material.dart';
import 'package:shrimp_farming/models/sensor_model.dart';
import 'package:lottie/lottie.dart';

class InfoCard extends StatelessWidget {
  final SensorModel sensor;
  const InfoCard({super.key, required this.sensor});

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(15),
      decoration: BoxDecoration(
        color: sensor.backgroundColor,
        borderRadius: BorderRadius.circular(25),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withOpacity(0.05),
            blurRadius: 10,
            offset: const Offset(0, 5),
          ),
        ],
      ),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          // 1. Icon Header
          Image.asset(
            sensor.icon,
            width: 35,
            height: 35,
            color: sensor.textColor, // Tự động đổi màu icon theo text
          ),
          const SizedBox(height: 10),

          // 2. Tiêu đề (Nhiệt độ, pH...)
          Text(
            sensor.title,
            textAlign: TextAlign.center,
            style: TextStyle(
              color: sensor.textColor,
              fontSize: 18,
              fontWeight: FontWeight.w500,
            ),
          ),

          const Spacer(),

          // 3. HIỂN THỊ GIÁ TRỊ (Nhiệt độ/pH hoặc Lottie)
          if (sensor.value != null)
            Column(
              children: [
                Text(
                  // Hiển thị 1 chữ số thập phân, thêm đơn vị tương ứng
                  "${sensor.value!.toStringAsFixed(1)}${_getUnit(sensor.title)}",
                  style: TextStyle(
                    color: sensor.textColor, // PHẢI CÓ DÒNG NÀY ĐỂ HIỆN CHỮ
                    fontSize: 32,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ],
            )
          else if (sensor.lottiePath != null)
            SizedBox(
              height: 70,
              child: Lottie.asset(
                sensor.lottiePath!,
                animate: sensor.isActive,
                repeat: true,
              ),
            ),

          const Spacer(),

          // 4. Trạng thái dưới cùng
          Text(
            sensor.isActive ? "Đang hoạt động" : "Đang dừng",
            style: TextStyle(
              color: sensor.textColor.withOpacity(0.8),
              fontSize: 12,
            ),
          ),
        ],
      ),
    );
  }

  // Hàm phụ để lấy đơn vị dựa theo tiêu đề
  String _getUnit(String title) {
    if (title.contains("Nhiệt độ")) return "°C";
    if (title.contains("pH")) return "";
    if (title.contains("ăn")) return "kg";
    return "";
  }
}