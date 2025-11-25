# WiBLE Tests

This directory contains tests for the WiBLE library.

## Running Tests

### Prerequisites
- **ESP32 Toolchain**: The library depends on the ESP32 Arduino Core (`WiFi.h`, `BLEDevice.h`, etc.).
- **PlatformIO** or **Arduino IDE**: Recommended for compiling and running tests on actual hardware.

### Local Verification (Linux/Host)
We have provided a `mocks` directory to allow basic syntax checking on a host machine (Linux/macOS).
However, full compilation requires the actual ESP32 SDK headers.

To verify the State Machine logic (which is platform-independent):
```bash
g++ -c src/StateManager.cpp tests/mocks/Arduino.cpp -I src -I src/utils -I tests/mocks -DARDUINO=100 -std=c++17
```

### Mocks
The `tests/mocks` directory contains minimal definitions of Arduino and ESP32 classes to facilitate partial local compilation. These are **not** complete implementations and are only for syntax verification.
