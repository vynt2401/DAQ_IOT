import 'package:flutter/material.dart';

class SensorModel {
  final String title;
  final String icon;
  final Color backgroundColor;
  final Color textColor;
  final bool isActive;
  final double? value;
  final String? lottiePath;

  SensorModel({
    required this.title,
    required this.icon,
    required this.backgroundColor,
    this.textColor = Colors.white,
    this.isActive = true,
    this.value,
    this.lottiePath,
  });

  // Sửa lại hàm copyWith ở đây
  SensorModel copyWith({
    String? title,           // Dùng dấu ? và dấu phẩy ,
    String? icon,
    Color? backgroundColor,
    Color? textColor,
    bool? isActive,
    double? value,
    String? lottiePath,
  }) {
    return SensorModel(
      // Nếu giá trị truyền vào là null, nó sẽ lấy giá trị cũ (this.property)
      title: title ?? this.title,
      icon: icon ?? this.icon,
      backgroundColor: backgroundColor ?? this.backgroundColor,
      textColor: textColor ?? this.textColor,
      isActive: isActive ?? this.isActive,
      value: value ?? this.value,
      lottiePath: lottiePath ?? this.lottiePath,
    );
  }
}