### WiBLE – Your Gateway Between BLE and Wi-Fi

---

## Overview

WiBLE is a next-generation, cross-platform provisioning framework built for modern IoT devices. It unifies **Bluetooth Low Energy (BLE)** and **Wi-Fi** communication into a single, secure, and developer-friendly library that enables **seamless device setup**, **network onboarding**, and **scalable connectivity management**.

Unlike traditional provisioning systems that rely solely on Wi-Fi AP modes or basic BLE message passing, WiBLE introduces a **hybrid connectivity model** — BLE for setup and configuration, Wi-Fi for real-time communication and data transfer — all wrapped in a single, developer-friendly API.

**Tagline:** Your Gateway Between BLE and Wi-Fi

---

## Objective

To build a **lightweight yet powerful provisioning system** capable of:
- Handling both **BLE** and **Wi-Fi** in one unified library.
- Providing **secure encrypted communication** using ECDH + AES.
- Supporting the **ESP32**, **ESP32-C6**, and similar IoT boards.
- Delivering an **event-driven, developer-friendly API**.
- Scaling from small prototypes to industrial IoT deployments.

---

## Core Features

### 1. Hybrid Connectivity (BLE + Wi-Fi)
- BLE for discovery, pairing, and data transfer.
- Wi-Fi for cloud communication and real-time operation.
- Automatic switch between BLE ↔ Wi-Fi modes.

### 2. Secure Provisioning
- Uses **ECDH** for key exchange and **AES-256** for data encryption.
- Supports optional **PIN-based authentication**.
- Prevents credential sniffing or replay attacks.

### 3. Unified API
- Simple, modular, and event-driven C++ API.
- Compatible with both **Arduino** and **ESP-IDF** environments.
- Built-in events: `onBLEConnected()`, `onWiFiConnected()`, `onProvisionSuccess()`, `onWiFiError()`.

### 4. Wi-Fi Management
- Scan nearby Wi-Fi networks.
- Connect to provided SSID and password.
- Automatically reconnect on disconnection.
- Store credentials securely in **NVS/Flash**.

### 5. Developer Utilities
- Built-in logging system for BLE and Wi-Fi events.
- JSON/CBOR-based provisioning messages.
- CLI and GUI testing tools for developers.
- Extensible provisioning profiles (add tokens, calibration data, etc.).

---

## System Architecture

```
+----------------------------------------------------+
|               Mobile / Web Client                  |
|  - BLE Scanning & Pairing                          |
|  - Sends Wi-Fi credentials + metadata              |
|  - Displays provisioning progress                  |
+--------------------------+-------------------------+
                           |
                           |  Secure BLE Channel (ECDH + AES)
                           v
+----------------------------------------------------+
|           IoT Device (ESP32 / ESP32-C6)            |
|  - BLE Server (Provisioning Service)               |
|  - Wi-Fi Connection Manager                        |
|  - Security Layer + JSON Parser                    |
|  - Event System + Persistent Storage (NVS)         |
|  - Cloud Integrations (MQTT / REST)                |
+----------------------------------------------------+
                           |
                           |  Wi-Fi Connection
                           v
+----------------------------------------------------+
|                    Cloud / API                     |
|  - Device Registration                             |
|  - OTA, Telemetry, Analytics                       |
|  - User Dashboard / App                            |
+----------------------------------------------------+
```

---

## Example Use Case

**Scenario:** A user sets up a new GrowTower IoT device.
1. The device boots and advertises itself via **BLE**.
2. The mobile app connects and sends Wi-Fi credentials.
3. The device establishes a **secure BLE session**, stores credentials, and connects to Wi-Fi.
4. Upon success, BLE switches off (to save power), and the device operates via Wi-Fi.
5. The app confirms provisioning success and starts monitoring data.

All managed automatically by **WiBLE**.

---

## Innovation

- **Dual Connectivity Architecture:** Combines BLE and Wi-Fi in one intelligent stack.
- **Security-First Design:** Cryptographic protection by default.
- **Extensible Protocol:** Add your own provisioning data easily.
- **Developer-Centric:** Event-based, minimal setup, and portable.

---

## Example API

```cpp
#include "WiBLE.h"

WiBLE nc;

void setup() {
  Serial.begin(115200);

  nc.onBLEConnected([]() { Serial.println("BLE connected"); });
  nc.onWiFiConnected([](String ssid) { Serial.println("Connected to Wi-Fi: " + ssid); });
  nc.onProvisionSuccess([]() { Serial.println("Provisioning successful!"); });

  nc.begin({
    .deviceName = "WiBLE_ESP32C6",
    .enableSecurity = true,
    .enableWiFi = true
  });
}

void loop() {
  nc.loop();
}
```

---

## Planned Directory Structure

```
WiBLE/
├── src/
│   ├── BLEManager.cpp/.h
│   ├── WiFiManager.cpp/.h
│   ├── SecurityLayer.cpp/.h
│   ├── ProvisioningCore.cpp/.h
│   ├── Storage.cpp/.h
│   ├── Events.cpp/.h
│   └── WiBLE.h
├── examples/
│   ├── BasicProvisioning/
│   ├── SecureProvisioning/
│   └── MultiDeviceProvisioning/
└── README.md
```

---

## Future Enhancements

- Support for **Matter / Thread** provisioning.  
- Cross-platform SDKs for **Flutter**, **React Native**, and **Web Bluetooth**.  
- Cloud integrations (HiveMQ, AWS IoT, Firebase).  
- OTA (Over-The-Air) firmware updates.  
- Device heartbeat and telemetry APIs.  

---

## Mission Statement

> “WiBLE aims to redefine IoT provisioning by merging BLE and Wi-Fi into a single, secure, and scalable connectivity framework — empowering developers to create seamless, user-friendly, and future-proof IoT experiences.”

