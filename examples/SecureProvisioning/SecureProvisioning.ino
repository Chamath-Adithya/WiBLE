/*
 * Secure Provisioning Example
 * 
 * This example demonstrates how to use WiBLE's secure provisioning mode.
 * It uses ECDH (Elliptic Curve Diffie-Hellman) for key exchange and 
 * AES-256 for encrypting credentials.
 * 
 * Security Features:
 * - Perfect Forward Secrecy via ECDH
 * - AES-256 Encryption for all sensitive data
 * - Proof of Possession (PoP) optional
 * 
 * Author: Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;

// Global provisioner instance
WiBLE provisioner;

void setup() {
    Serial.begin(115200);
    
    // 1. Configure Provisioning
    ProvisioningConfig config;
    config.deviceName = "WiBLE-Secure";
    
    // CRITICAL: Set security level to SECURE
    // This enables ECDH key exchange and AES-256 encryption
    config.securityLevel = SecurityLevel::SECURE;
    
    // Optional: Set a Proof of Possession (PoP) string
    // The user must enter this in the app to authenticate
    // config.proofOfPossession = "123456"; 
    
    // 2. Initialize
    if (!provisioner.begin(config)) {
        Serial.println("Failed to initialize WiBLE!");
        return;
    }
    
    // 3. Register Callbacks
    
    // Called when WiFi is successfully connected
    provisioner.onWiFiConnected([](String ssid, String ip) {
        Serial.println("--------------------------------");
        Serial.printf("Securely Connected to: %s\n", ssid.c_str());
        Serial.printf("IP Address: %s\n", ip.c_str());
        Serial.println("--------------------------------");
    });
    
    // Called when provisioning starts (BLE connected)
    provisioner.onProvisioningStart([]() {
        Serial.println("Device connected via BLE. Starting secure session...");
    });
    
    // Called on errors
    provisioner.onError([](ErrorCode code, String msg, bool canRetry) {
        Serial.printf("Error [%d]: %s\n", (int)code, msg.c_str());
    });
    
    // 4. Start Provisioning
    Serial.println("Starting Secure Provisioning...");
    provisioner.startProvisioning();
}

void loop() {
    // CRITICAL: Must be called in the loop to handle BLE/WiFi events
    provisioner.loop();
}
