/**
 * AdvancedProvisioning.ino
 * 
 * Demonstrates advanced features of WiBLE:
 * - Callbacks for every state change and event
 * - Custom data handling (sending/receiving)
 * - Security configuration
 * - Manual control
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// ============================================================================
// CALLBACKS
// ============================================================================

void onStateChange(ProvisioningState oldState, ProvisioningState newState) {
    Serial.printf("[Callback] State Changed: %d -> %d\n", (int)oldState, (int)newState);
}

void onWiFiConnected(String ssid, String ip) {
    Serial.printf("[Callback] WiFi Connected! SSID: %s, IP: %s\n", ssid.c_str(), ip.c_str());
    
    // Example: Send a "Hello" message back to the phone app via WiFi (if supported)
    // provisioner.sendWiFiData("/status", "Online");
}

void onError(ErrorCode code, String msg, bool canRetry) {
    Serial.printf("[Callback] Error %d: %s (Retryable: %s)\n", 
                  (int)code, msg.c_str(), canRetry ? "Yes" : "No");
}

void onDataReceived(const uint8_t* data, size_t len) {
    Serial.print("[Callback] Received Custom Data: ");
    for(size_t i=0; i<len; i++) Serial.print((char)data[i]);
    Serial.println();
    
    // Echo back
    provisioner.sendBLEData(data, len);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting Advanced WiBLE Example...");

    // 1. Configuration
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Advanced";
    config.securityLevel = SecurityLevel::SECURE; // Require pairing
    config.proofOfPossession = "123456";          // PIN for pairing (if supported)
    config.wifiMaxRetries = 5;
    
    // 2. Register Callbacks BEFORE begin()
    provisioner.onStateChange(onStateChange);
    provisioner.onWiFiConnected(onWiFiConnected);
    provisioner.onError(onError);
    provisioner.onDataReceived(onDataReceived);
    
    // 3. Initialize
    if (!provisioner.begin(config)) {
        Serial.println("Initialization Failed!");
        while(1);
    }

    // 4. Set Custom Data (e.g., Device Serial Number)
    // This can be read by the app
    // provisioner.setCustomData("serial", "SN-99887766");
    
    Serial.println("Setup Complete. Waiting for interaction...");
}

void loop() {
    provisioner.loop();
    
    // Simulate some application logic
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        if (provisioner.isProvisioned()) {
            Serial.println("Device is running in PROVISIONED mode.");
        } else {
            Serial.println("Device is waiting for provisioning...");
        }
        lastPrint = millis();
    }
}
