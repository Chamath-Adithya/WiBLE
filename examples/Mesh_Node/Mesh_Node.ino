/**
 * Mesh_Node.ino
 * 
 * Demonstrates Method 5: Mesh Setup.
 * 
 * Scenario:
 * Use WiBLE to provision a device with:
 * 1. WiFi Credentials (Standard)
 * 2. Mesh Network Key & App Key (Custom Data)
 * 
 * Once provisioned, the device "joins" the mesh (simulated).
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

struct MeshCredentials {
    char netKey[33];
    char appKey[33];
    uint16_t unicastAddr;
};

MeshCredentials meshCreds;
bool meshConfigured = false;

void onDataReceived(const uint8_t* data, size_t len) {
    // Expecting JSON or binary struct with Mesh Keys
    // For simplicity, let's assume it's a JSON string: {"netKey":"...", "appKey":"...", "addr":1}
    // We'll just print it for now.
    
    String payload = "";
    for(size_t i=0; i<len; i++) payload += (char)data[i];
    
    Serial.println("[Mesh] Received Configuration: " + payload);
    
    // Parse (Mock parsing)
    if (payload.indexOf("netKey") > 0) {
        Serial.println("[Mesh] Network Key received.");
        meshConfigured = true;
        // Save to NVS...
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Mesh Node Provisioning...");

    ProvisioningConfig config;
    config.deviceName = "WiBLE_Mesh_Node";
    
    provisioner.onDataReceived(onDataReceived);
    
    provisioner.begin(config);
}

void loop() {
    provisioner.loop();
    
    if (meshConfigured) {
        static uint32_t lastLog = 0;
        if (millis() - lastLog > 5000) {
            Serial.println("[Mesh] Device is participating in the Mesh Network...");
            lastLog = millis();
        }
    }
}
