# Shrimp Farming IoT Monitoring System
### Ứng dụng Điều khiển & Giám sát Ao nuôi Tôm
## Mô tả Ứng dụng

Ứng dụng Shrimp Farming IoT Dashboard là giao diện web toàn diện cho hệ thống giám sát ao nuôi tôm thông minh. Ứng dụng kết nối trực tiếp với server IoT để hiển thị dữ liệu thời gian thực và cung cấp khả năng điều khiển từ xa các thiết bị trong hệ thống.

## Tính năng chính
### Giám sát Thời gian Thực
Hiển thị trực quan các thông số môi trường: Nhiệt độ, pH, DO

Biểu đồ theo thời gian với khả năng zoom và lọc dữ liệu

Trạng thái thiết bị cập nhật liên tục (Online/Offline)

Cảnh báo trực tiếp khi thông số vượt ngưỡng

## Điều khiển Từ xa
Bật/Tắt thiết bị: Máy bơm, quạt oxy, hệ thống cho ăn

Chế độ tự động: Điều khiển dựa trên ngưỡng thông số

Lập lịch hoạt động: Hẹn giờ cho các thiết bị

Điều khiển thủ công: Override chế độ tự động khi cần

## Phân tích & Báo cáo
Xuất dữ liệu sang CSV/Excel

Báo cáo hàng ngày/tuần/tháng

Phân tích xu hướng và dự báo

Thống kê hiệu suất thiết bị

## Hệ thống Cảnh báo
Cảnh báo đa cấp độ (Cao/Trung bình/Thấp)

Đa kênh thông báo: Dashboard, Email, SMS

Log cảnh báo với lịch sử đầy đủ

Cấu hình ngưỡng linh hoạt

## SYSTEM OVERVIEW

1. Dashboard Chính
Overview Cards: Tổng quan nhanh tất cả thông số

Real-time Charts: Biểu đồ cập nhật liên tục

Device Status Panel: Trạng thái các thiết bị

Alert Center: Các cảnh báo đang hoạt động

2. Trang Điều khiển
Virtual Control Panel: Nút bật/tắt thiết bị

Manual Control Mode: Điều khiển thủ công

Auto Mode Settings: Cấu hình ngưỡng tự động

Schedule Manager: Lập lịch thiết bị

3. Trang Lịch sử
Data Table: Xem dữ liệu theo bảng

Time Range Selector: Chọn khoảng thời gian

Export Options: Xuất dữ liệu đa định dạng

Filter & Search: Lọc dữ liệu theo tiêu chí

4. Trang Cài đặt
User Management: Quản lý người dùng

Device Configuration: Thêm/xóa thiết bị

Alert Settings: Cấu hình cảnh báo

System Preferences: Cài đặt hệ thống

## Công nghệ Sử dụng
Frontend
HTML5/CSS3/JavaScript - Cấu trúc và styling

Bootstrap 5 - Responsive design

Chart.js - Visualization

Socket.IO Client - Real-time updates

jQuery/AJAX - API communication

Backend (Integration)
Flask - Web framework

Flask-SocketIO - Real-time communication

REST API - Data exchange với server IoT

Jinja2 Templating - Dynamic content

Kết nối với Server IoT
WebSocket Connection - Real-time data stream

REST API Endpoints - Device control & configuration

Authentication - Session-based security

Data Synchronization - Bi-directional sync

## Cách thức Hoạt động
Luồng dữ liệu từ ESP32 → App
ESP32 gửi sensor data qua MQTT

Server IoT xử lý và lưu database

App kết nối WebSocket nhận data real-time

Frontend cập nhật UI và charts

Luồng điều khiển từ App → ESP32
User click control button trong app

App gửi command qua REST API

Server IoT publish MQTT command

ESP32 nhận và thực thi lệnh

ESP32 gửi confirmation back

Đồng bộ Trạng thái
Heartbeat Monitoring: Kiểm tra thiết bị online

State Persistence: Lưu trạng thái vào database

Connection Recovery: Tự động kết nối lại khi mất

Data Buffering: Đệm dữ liệu khi offline

## Tính năng Nâng cao
Multi-User Support
Role-based Access: Admin/User/Guest

Permission Management: Phân quyền chi tiết

Activity Logging: Ghi nhận hành động user

Mobile Optimization
Responsive Design: Tương thích mọi màn hình

Touch Controls: Tối ưu cho mobile

PWA Ready: Có thể cài đặt như app native

Internationalization
Multi-language Support: Tiếng Việt/English

Localization: Format số, thời gian theo region

RTL Support: Hỗ trợ ngôn ngữ viết phải-sang-trái

Performance Features
Lazy Loading: Tải trang theo nhu cầu

Data Caching: Cache dữ liệu local

Connection Pooling: Quản lý kết nối hiệu quả

Compression: Nén dữ liệu truyền tải

## Bảo mật
Authentication & Authorization
Session Management: Secure session tokens

Password Encryption: BCrypt hashing

CSRF Protection: Form submission security

API Rate Limiting: Prevent abuse

Data Security
HTTPS Enforcement: All traffic encrypted

Input Validation: Sanitize user inputs

XSS Prevention: Escape dangerous characters

SQL Injection Protection: Parameterized queries

Privacy Features
Data Anonymization: Ẩn thông tin nhạy cảm

Access Logs: Ghi nhận truy cập

GDPR Compliance: Tuân thủ quy định bảo mật

Data Retention Policies: Tự động xóa dữ liệu cũ

## Hiệu suất & Scalability
Tối ưu Performance
Minified Assets: CSS/JS compression

Image Optimization: WebP format + lazy load

CDN Integration: Phân phối static content

Browser Caching: Cache headers optimization

Scalability Features
Horizontal Scaling: Support multiple server instances

Load Balancing: Distribute traffic evenly

Database Sharding: Scale data storage

Microservices Ready: Modular architecture

## Development & Deployment
Development Setup
bash
```
# Clone repository
git clone https://github.com/vynt2401/DAQ_IOT
cd DAQ_IOT/app/shrimp_farming
```
# Install dependencies (if any)
# Note: App này chủ yếu là frontend, 
# kết nối với server IoT backend

# Run development server
```
python app.py  # Hoặc server tương ứng
```
### Build Process
Static Asset Compilation: CSS/JS minification

Template Pre-compilation: Jinja2 optimization

Dependency Bundling: Package management

Version Tagging: Semantic versioning

Deployment Options
Traditional Hosting: Apache/Nginx + WSGI

Containerized: Docker + Kubernetes

Cloud Platforms: AWS, GCP, Azure, Vercel

Edge Computing: CDN edge locations

## Monitoring & Maintenance
### Health Checks
API Endpoint Monitoring: /health, /status

Database Connectivity: Connection pool status

External Service Dependencies: MQTT, Email, SMS

Performance Metrics: Response time, error rate

### Logging System
Application Logs: User actions, errors

Access Logs: HTTP requests

Error Tracking: Stack traces, debugging info

Audit Trails: Security-related events

### Maintenance Features
Backup & Restore: Database và cấu hình

Update Management: OTA updates (nếu có)

Configuration Management: Environment variables

Rollback Capability: Version rollback khi cần

## Tích hợp & Mở rộng
Third-party Integrations
Weather APIs: Dự báo thời tiết

Market Data APIs: Giá tôm, thức ăn

Notification Services: Twilio, SendGrid, Firebase

Analytics Tools: Google Analytics, Mixpanel

### Custom Modules
Plugin System: Thêm tính năng mới

Webhook Support: Kích hoạt external services

API Gateway: Unified API interface

Event Bus: Internal communication

### IoT Ecosystem
Multiple Device Types: Hỗ trợ nhiều loại sensor

Protocol Support: MQTT, HTTP, CoAP

Gateway Integration: Kết nối nhiều ESP32

Firmware Management: OTA updates cho thiết bị

### User Documentation
Getting Started Guide: Hướng dẫn cơ bản

User Manual: Chi tiết sử dụng

FAQ Section: Câu hỏi thường gặp

Video Tutorials: Hướng dẫn trực quan

