# Frequently Asked Questions (FAQ)

## 1. How do I provision an ESP32 using Bluetooth?
WiBLE makes it extremely easy. Include the library, configure your security level, and call `provisioner.startProvisioning()`. The device will advertise via BLE, allowing a mobile app to connect and securely send WiFi credentials.

## 2. Is WiBLE secure?
Yes. WiBLE utilizes ECDH (Elliptic Curve Diffie-Hellman) for perfect forward secrecy during key exchange, and AES-256 for encrypting credentials.

## 3. Which boards are supported?
WiBLE supports all ESP32 variants that include Bluetooth Low Energy capabilities, including ESP32-C3, ESP32-S3, and ESP32-C6.

## 4. Can I use WiBLE with PlatformIO and Arduino IDE?
Absolutely! WiBLE is designed to work seamlessly with both PlatformIO and the Arduino IDE framework.

## 5. Why am I running out of memory/partition space?
WiBLE bundles BLE, WiFi, and advanced cryptographic libraries. This exceeds the standard 1.3MB app partition size. You MUST use a larger partition scheme like "Huge APP (3MB No OTA)" in Arduino IDE, or `board_build.partitions = huge_app.csv` in PlatformIO.
