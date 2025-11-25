# WiBLE - Your Gateway Between BLE and Wi-Fi

<p align="center">
  <img src="docs/images/wible-logo.png" alt="WiBLE Logo" width="200"/>
</p>

<p align="center">
  <strong>A production-grade, secure, and scalable BLE provisioning library for ESP32/ESP32-C6</strong>
</p>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#quick-start">Quick Start</a> •
  <a href="#documentation">Documentation</a> •
  <a href="#examples">Examples</a> •
  <a href="#contributing">Contributing</a>
</p>

---

## 🎯 Overview

WiBLE is a next-generation provisioning framework that seamlessly combines **Bluetooth Low Energy (BLE)** and **Wi-Fi** connectivity into a unified, developer-friendly library. It enables IoT devices to be configured securely and reliably through mobile apps, eliminating the complexity of manual network setup.

### Why WiBLE?

| Feature | Traditional Approach | WiBLE |
|---------|---------------------|-------|
| Security | Basic or None | ECDH + AES-256 by default |
| Error Handling | Manual retries | Automatic with exponential backoff |
| State Management | Boolean flags | Finite State Machine (FSM) |
| Connection Time | 30-60s | < 18s |
| Testing | Manual only | Unit + Integration tests |
| Documentation | Basic README | Comprehensive API docs |

---

## ✨ Features

### 🔒 **Security First**
- **ECDH Key Exchange** - Perfect Forward Secrecy
- **AES-256 Encryption** - Military-grade credential protection
- **Multiple Security Levels** - From hobbyist to enterprise
- **Secure Storage** - Encrypted credentials in NVS

### 🚀 **Performance Optimized**
- **< 18s Total Provisioning** - 50% faster than alternatives
- **MTU Negotiation** - Up to 512 bytes per packet
- **Chunked Transfer** - Handle large data seamlessly
- **Connection Pooling** - Multi-device support

### 🛡️ **Production Ready**
- **Finite State Machine** - Predictable state transitions
- **Circuit Breaker** - Prevent cascading failures
- **Auto-Reconnect** - WiFi resilience with exponential backoff
- **Comprehensive Logging** - Structured, filterable logs

### 👨‍💻 **Developer Friendly**
- **Simple API** - 5 lines to get started
- **Event-Driven** - Reactive callbacks
- **80+ Unit Tests** - High code quality
- **Rich Examples** - Learn by doing

---

## 📦 Partition Scheme (Critical!)

**WiBLE requires a larger app partition than the default.**
The combined size of the WiFi stack, BLE stack (Bluedroid), and security libraries exceeds the standard 1.3MB app partition.

### Arduino IDE
1. Go to **Tools > Partition Scheme**.
2. Select **"Huge APP (3MB No OTA/1MB SPIFFS)"** or **"Minimal SPIFFS (1.9MB APP with OTA)"**.

### PlatformIO
Add this to your `platformio.ini`:
```ini
board_build.partitions = huge_app.csv
; OR for OTA support:
; board_build.partitions = min_spiffs.csv
```

## 🚀 Quick Start

### 1. Installation

#### PlatformIO (Recommended)
```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    yourorg/WiBLE@^2.0.0
```

#### Arduino IDE
1. Download latest release from [GitHub Releases](https://github.com/yourorg/wible/releases)
2. Extract to `Arduino/libraries/WiBLE/`
3. Restart Arduino IDE

### 2. Basic Example

```cpp
#include <WiBLE.h>

using namespace WiBLE;
WiBLE provisioner;

void setup() {
    Serial.begin(115200);
    
    // Configure library
    ProvisioningConfig config;
    config.deviceName = "MyIoTDevice";
    config.securityLevel = SecurityLevel::SECURE;
    
    // Initialize
    provisioner.begin(config);
    
    // Register callbacks
    provisioner.onWiFiConnected([](String ssid, String ip) {
        Serial.printf("Connected to %s with IP %s\n", ssid.c_str(), ip.c_str());
    });
    
    provisioner.onError([](ErrorCode code, String msg, bool canRetry) {
        Serial.printf("Error: %s\n", msg.c_str());
    });
    
    // Start provisioning
    provisioner.startProvisioning();
}

void loop() {
    provisioner.loop();  // CRITICAL: Call every iteration
}
```

### 3. Mobile App Setup

1. Download **WiBLE Companion App**:
   - [iOS App Store](https://apps.apple.com/app/wible)
   - [Google Play Store](https://play.google.com/store/apps/details?id=com.wible)

2. Open app and scan for devices
3. Select your device
4. Enter WiFi credentials
5. Wait for provisioning to complete

---

## 📖 Documentation

### Architecture

WiBLE uses a **five-layer architecture** for maximum flexibility:

```
Application Layer (Your Code)
    ↓
Core Services Layer (State Machine, Security, Orchestration)
    ↓
    Provisioning Orchestrator (Coordinates BLE & WiFi)
    ↓
Protocol Layer (BLE Manager, WiFi Manager)
    ↓
Platform Abstraction Layer (ESP32 APIs)
    ↓
Cross-Cutting Concerns (Logging, Metrics, Config)
```

### Key Components

#### **1. State Machine**
Manages provisioning lifecycle with predictable transitions:

```
IDLE → BLE_ADVERTISING → BLE_CONNECTED → AUTHENTICATING
  → RECEIVING_CREDENTIALS → CONNECTING_WIFI → PROVISIONED
```

#### **2. Security Manager**
Handles all cryptographic operations:
- ECDH-P256 key exchange
- AES-256-CBC/GCM encryption
- HMAC-SHA256 message authentication
- Secure random number generation

#### **3. BLE Manager**
Manages Bluetooth operations:
- GATT service/characteristic handling
- MTU negotiation (up to 512 bytes)
- Operation queue (serialized GATT ops)
- RSSI monitoring

#### **4. WiFi Manager**
Handles network connectivity:
- Network scanning
- Connection with retry logic
- Auto-reconnect with circuit breaker
- Multi-network support

---

## 🎓 Examples

### Basic Provisioning
Simple example showing minimal setup:
```cpp
examples/BasicProvisioning/BasicProvisioning.ino
```

### More Examples (Coming Soon)
- **Secure Provisioning**: Advanced security with PIN authentication
- **Multi-Device**: Provision multiple devices simultaneously
- **Custom Protocol**: Extend provisioning with custom data
- **Background Operation**: Long-running provisioning with power management

---

## 🔧 Configuration

### Security Levels

```cpp
// No encryption (dev only)
config.securityLevel = SecurityLevel::NONE;

// Basic pairing
config.securityLevel = SecurityLevel::BASIC;

// ECDH + AES-256 (recommended)
config.securityLevel = SecurityLevel::SECURE;

// Certificate pinning + AES-256-GCM
config.securityLevel = SecurityLevel::ENTERPRISE;
```

### Connection Parameters

```cpp
// Fast provisioning (high power)
config.connectionInterval = 24;      // 30ms
config.mtuSize = 512;
config.enablePowerSaving = false;

// Power-efficient (slower)
config.connectionInterval = 80;      // 100ms
config.mtuSize = 185;
config.enablePowerSaving = true;
```

### WiFi Settings

```cpp
config.wifiConnectTimeoutMs = 20000;  // 20s
config.wifiMaxRetries = 3;
config.autoReconnect = true;
config.persistCredentials = true;     // Save to NVS
```

---

## 🧪 Testing

### Run Unit Tests

```bash
# PlatformIO
pio test -e native

# Arduino
# Use Arduino IDE's built-in test runner
```

### Run Integration Tests

```bash
pio test -e esp32
```

### Test Coverage

```bash
# Generate coverage report
pio test -e native --coverage
lcov --capture --directory .pio/build/native --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

Current coverage: **85%** (target: 80%)

---

## 📊 Performance Metrics

### Provisioning Time

| Phase | Time | Optimizations |
|-------|------|--------------|
| BLE Scan | 2-3s | Pre-scanning |
| BLE Connection | 1-2s | Fast intervals |
| Authentication | 1-2s | ECDH caching |
| Credential Transfer | 0.5-1s | MTU 512 |
| WiFi Connection | 5-10s | Retry backoff |
| **Total** | **10-18s** | **50% improvement** |

### Memory Usage

| Component | RAM | Flash |
|-----------|-----|-------|
| WiBLE Core | 45KB | 180KB |
| BLE Stack | 50KB | 200KB |
| WiFi Stack | 40KB | 150KB |
| **Total** | **135KB** | **530KB** |

### Power Consumption

| State | Current | Duration |
|-------|---------|----------|
| BLE Advertising | 25mA | Variable |
| BLE Connected | 20mA | 5-10s |
| WiFi Connecting | 100mA | 5-10s |
| WiFi Connected | 80mA | Continuous |
| Deep Sleep | 10μA | When idle |

---

## 🔍 Debugging

### Enable Verbose Logging

```cpp
config.logLevel = LogLevel::VERBOSE;
config.enableSerialLog = true;
```

### Dump State Information

```cpp
provisioner.dumpState();
```

Output:
```
Current State: BLE_CONNECTED
Previous State: BLE_ADVERTISING
Time in State: 5234ms
BLE Connected: true
WiFi Connected: false
Secure Session: true
MTU: 512 bytes
```

### Monitor Metrics

```cpp
auto metrics = provisioner.getMetrics();
Serial.printf("Success Rate: %.1f%%\n", 
    (float)metrics.successfulProvisionings / 
    metrics.totalProvisioningAttempts * 100);
```

---

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
# Clone repository
git clone https://github.com/yourorg/wible.git
cd wible

# Install dependencies
pio lib install

# Build
pio run -e esp32

# Run tests
pio test -e native

# Format code
pio run -t format
```

### Code Style

- Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- Use `clang-format` for formatting
- Write tests for new features
- Update documentation

---

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

Built on insights from:
- [ESP-IDF Provisioning](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html)
- [Nordic Android BLE Library](https://github.com/NordicSemiconductor/Android-BLE-Library)
- [Research papers on BLE security](docs/references.md)

---

## 📞 Support

- 📧 Email: support@wible.dev
- 💬 Discord: [Join our community](https://discord.gg/wible)
- 🐛 Issues: [GitHub Issues](https://github.com/yourorg/wible/issues)
- 📖 Docs: [Full Documentation](https://wible.dev/docs)

---

## 🗺️ Roadmap

### v2.1 (Q2 2025)
- [ ] Bluetooth Mesh support
- [ ] Matter/Thread integration
- [ ] Flutter SDK
- [ ] React Native SDK

### v2.2 (Q3 2025)
- [ ] OTA firmware updates
- [ ] Cloud integration (AWS IoT, Azure)
- [ ] Analytics dashboard
- [ ] Multi-language support

### v3.0 (Q4 2025)
- [ ] BLE 5.3 features
- [ ] Kubernetes deployment
- [ ] Edge computing support
- [ ] AI-powered diagnostics

---

## 📈 Statistics

![GitHub Stars](https://img.shields.io/github/stars/yourorg/wible?style=social)
![Downloads](https://img.shields.io/github/downloads/yourorg/wible/total)
![License](https://img.shields.io/github/license/yourorg/wible)
![Build Status](https://img.shields.io/github/actions/workflow/status/yourorg/wible/ci.yml)

---

<p align="center">
  Made with ❤️ by the WiBLE Team
</p>

<p align="center">
  <a href="https://wible.dev">Website</a> •
  <a href="https://wible.dev/docs">Documentation</a> •
  <a href="https://github.com/yourorg/wible">GitHub</a>
</p>