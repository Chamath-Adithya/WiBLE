# WiBLE Examples

This directory contains a comprehensive set of examples demonstrating how to use the WiBLE library.

## 🔰 Starting Point

| Example | Description | Recommended For |
| :--- | :--- | :--- |
| **[SimplestProvisioning](SimplestProvisioning/SimplestProvisioning.ino)** | The absolute minimum - just 20 lines! | **First-time users!** Quick testing and learning. |
| **[BasicProvisioning](BasicProvisioning/BasicProvisioning.ino)** | A complete, production-ready template with logging, CLI, and error handling. | **Production apps!** Use this as the base for your commercial product. |

## 🚀 Advanced Features

| Example | Description | Key Features |
| :--- | :--- | :--- |
| **[AdvancedProvisioning](AdvancedProvisioning/AdvancedProvisioning.ino)** | Demonstrates specific advanced APIs. | Security (PIN), Custom Data, Callbacks. |
| **[DeepSleep](DeepSleep/DeepSleep.ino)** | Power management patterns. | State persistence, Deep Sleep integration. |
| **[WebClient](WebClient/WebClient.ino)** | Browser-based provisioning. | Web Bluetooth API (includes `index.html`). |

## 🔌 Connection Methods

| Example | Description | Method |
| :--- | :--- | :--- |
| **[BLE_Client](BLE_Client/BLE_Client.ino)** | Connect to other BLE devices. | GATT Client |
| **[Status_Broadcaster](Status_Broadcaster/Status_Broadcaster.ino)** | Broadcast data without connection. | Advertising (Beacon/Custom) |
| **[GatewayMode](GatewayMode/GatewayMode.ino)** | Scan for devices while being provisionable. | Server + Client (Scanner) |
| **[BeaconMode](BeaconMode/BeaconMode.ino)** | Act as an iBeacon. | iBeacon |
| **[Mesh_Node](Mesh_Node/Mesh_Node.ino)** | Simulates Mesh network provisioning. | Custom Data Provisioning |

## ☁️ Cloud & IoT

| Example | Description | Integration |
| :--- | :--- | :--- |
| **[SensorDashboard](SensorDashboard/SensorDashboard.ino)** | Bidirectional data: 4 sensors → app, 2 LEDs ← app. | Custom Protocol (see PROTOCOL.md) |
| **[MQTT_Client](MQTT_Client/MQTT_Client.ino)** | Full IoT flow: Provision -> WiFi -> MQTT. | `PubSubClient` |
| **[AWS_IoT](AWS_IoT/AWS_IoT.ino)** | Secure Cloud connectivity. | AWS IoT Core, Certificates |
| **[OTA_Update](OTA_Update/OTA_Update.ino)** | Wireless Firmware Update. | `Update` library |

## 💡 Which one should I choose?

1.  **Just Learning?** Start with `SimplestProvisioning.ino` (20 lines!).
2.  **Building a Product?** Copy `BasicProvisioning.ino` (production template).
3.  **Need High Security?** Check `AdvancedProvisioning` for PIN pairing and `AWS_IoT` for certs.
4.  **Battery Powered?** Study `DeepSleep`.
5.  **No Mobile App?** Use `WebClient`.
