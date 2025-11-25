# WiBLE User Guide

Welcome to WiBLE! This guide will help you integrate robust BLE provisioning into your ESP32 projects.

## 📚 Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Core Concepts](#core-concepts)
5. [Advanced Features](#advanced-features)
6. [Troubleshooting](#troubleshooting)

---

## 1. Introduction

WiBLE solves the "chicken and egg" problem of IoT: **How do you get a device onto WiFi when it doesn't have a keyboard?**

It turns your ESP32 into a BLE Access Point that a mobile app (or web page) can connect to. The app sends the WiFi credentials securely, and WiBLE handles the rest.

### Key Features
*   **Secure**: Uses ECDH Key Exchange and AES-256 Encryption.
*   **Fast**: Connects and provisions in under 15 seconds.
*   **Reliable**: Auto-reconnects to WiFi if the router restarts.

---

## 2. Installation

### Option A: PlatformIO (Recommended)
Add this to your `platformio.ini`:
```ini
lib_deps =
    yourorg/WiBLE @ ^2.0.0
```

### Option B: Arduino IDE
1.  Download the latest `.zip` release from GitHub.
2.  In Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...**
3.  Select the downloaded file.

---

## 3. Quick Start

The fastest way to get started is using the **BasicProvisioning** example.

1.  Open `examples/BasicProvisioning/BasicProvisioning.ino`.
2.  Flash it to your ESP32.
3.  Open the Serial Monitor (115200 baud).
4.  Use a BLE Scanner app (like nRF Connect) or the [WiBLE Web Client](../examples/WebClient/index.html) to connect.

### Minimal Code
```cpp
#include <WiBLE.h>

using namespace WiBLE;
WiBLE provisioner;

void setup() {
    Serial.begin(115200);
    
    ProvisioningConfig config;
    config.deviceName = "My_Device";
    
    provisioner.begin(config);
}

void loop() {
    provisioner.loop(); // Don't forget this!
}
```

---

## 4. Core Concepts

### The Lifecycle
WiBLE manages your device's connectivity state automatically:
1.  **Idle**: Device starts up.
2.  **Advertising**: Broadcasting "I am here, configure me!".
3.  **Connected**: Phone connects to device.
4.  **Provisioned**: WiFi credentials received and saved.
5.  **WiFi Connected**: Device is online.

### Security Levels
You can configure how secure the connection is:
*   `SecurityLevel::NONE`: No encryption (Dev only).
*   `SecurityLevel::BASIC`: Simple pairing.
*   `SecurityLevel::SECURE`: **Default**. ECDH Key Exchange + AES-256.

---

## 5. Advanced Features

### 📡 Broadcasting (Beacon Mode)
Want your device to act as an iBeacon when it's not being configured?
```cpp
// UUID, Major, Minor
provisioner.startBeaconMode("1234-5678-...", 1, 100);
```

### 🔍 Gateway Mode (Scanning)
Want to scan for other BLE devices while connected to WiFi?
```cpp
provisioner.scanForDevices(5, [](const String& addr, int rssi, const String& data){
    Serial.printf("Found device: %s (%d dBm)\n", addr.c_str(), rssi);
});
```

### ☁️ Cloud Integration (MQTT)
WiBLE plays nicely with standard libraries like `PubSubClient`.
See `examples/MQTT_Client` for a full pattern on how to:
1.  Provision via BLE.
2.  Connect to WiFi.
3.  Connect to MQTT Broker.

---

## 6. Troubleshooting

### "Device not showing up in scan"
*   Ensure `provisioner.loop()` is being called in your main loop.
*   Check if the device is already provisioned (it stops advertising by default when provisioned). Use `provisioner.clearProvisioning()` to reset.

### "Compilation Error: BLEDevice.h not found"
*   Ensure you are selecting an **ESP32** board in your IDE. This library does not support Arduino Uno/Nano.

### "WiFi connects but disconnects immediately"
*   Check your power supply. WiFi spikes can brown-out the ESP32.
*   Verify the antenna is connected (if using an external one).
