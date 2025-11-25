/**
 * GatewayMode.ino
 * 
 * Demonstrates the "Dual Role" capability of WiBLE.
 * The ESP32 acts as a Provisioning Server (waiting for mobile app)
 * AND as a Gateway Client (scanning for other BLE devices).
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// Callback for discovered devices
void onDeviceDiscovered(const String& address, int rssi, const String& mfgData) {
    Serial.printf("[SCAN] Device: %s | RSSI: %d | Data: %s\n", 
                  address.c_str(), rssi, mfgData.c_str());
                  
    // In a real gateway, you would filter for specific sensors here
    // and upload their data to the cloud via WiFi.
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting WiBLE Gateway Mode...");

    // Configure WiBLE
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Gateway";
    config.securityLevel = SecurityLevel::SECURE;
    
    // Initialize
    if (provisioner.begin(config)) {
        Serial.println("WiBLE Initialized!");
    } else {
        Serial.println("Initialization Failed!");
        return;
    }

    // Register Callbacks
    provisioner.onWiFiConnected([](String ssid, String ip) {
        Serial.printf("Gateway Connected to WiFi: %s (%s)\n", ssid.c_str(), ip.c_str());
    });
    
    // Access the internal BLE Manager to set up scanning
    // Note: In a production API, we would expose this via the main WiBLE class
    // For now, we assume the user has modified WiBLE.h to expose bleManager or added a wrapper
    // Since we are inside the library development, we can add a wrapper to WiBLE.h/cpp
    // OR we can just use the BLEManager singleton if it were one.
    // 
    // Wait! WiBLE.h doesn't expose bleManager publicly. 
    // We should add a `getBLEManager()` method or a `scan()` wrapper to WiBLE class.
    // Let's add a wrapper to WiBLE.h first.
}

void loop() {
    provisioner.loop();
    
    // Periodically scan for devices (every 10 seconds)
    static uint32_t lastScan = 0;
    if (millis() - lastScan > 10000) {
        Serial.println("Starting periodic scan...");
        provisioner.scanForDevices(5, onDeviceDiscovered);
        lastScan = millis();
    }
}
