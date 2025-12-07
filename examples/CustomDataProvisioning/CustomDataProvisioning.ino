/*
 * Custom Data Provisioning Example
 * 
 * This example demonstrates how to receive custom data (like API Keys, 
 * User IDs, or MQTT configurations) along with WiFi credentials.
 * 
 * Use Case:
 * You want to associate the device with a specific user account or 
 * configure it for a specific MQTT server during the initial setup.
 * 
 * Author: Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>
#include <ArduinoJson.h> // Requires ArduinoJson library

using namespace WiBLE;

WiBLEProvisioner provisioner;

// Structure to hold our custom config
struct DeviceConfig {
    String apiKey;
    String userId;
    String mqttServer;
    int mqttPort;
};

DeviceConfig myConfig;

void setup() {
    Serial.begin(115200);
    
    ProvisioningConfig config;
    config.deviceName = "WiBLE-Custom";
    config.securityLevel = SecurityLevel::SECURE;
    
    if (!provisioner.begin(config)) {
        Serial.println("Failed to initialize!");
        return;
    }
    
    // 1. Register Callback for Custom Data
    // The app sends a JSON string: {"apiKey":"abc", "userId":"123", "mqtt":"broker.hivemq.com"}
    provisioner.onCustomDataReceived([](String data) {
        Serial.println("Received Custom Data:");
        Serial.println(data);
        
        // Parse JSON (using ArduinoJson v6)
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, data);
        
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }
        
        // Save to our config struct
        myConfig.apiKey = doc["apiKey"].as<String>();
        myConfig.userId = doc["userId"].as<String>();
        myConfig.mqttServer = doc["mqtt"].as<String>();
        
        Serial.printf("API Key: %s\n", myConfig.apiKey.c_str());
        Serial.printf("User ID: %s\n", myConfig.userId.c_str());
        
        // In a real app, you would save this to NVS/Preferences here
    });
    
    provisioner.onWiFiConnected([](String ssid, String ip) {
        Serial.printf("Connected to %s (IP: %s)\n", ssid.c_str(), ip.c_str());
        
        // Now we can use the custom data to connect to our backend
        if (myConfig.apiKey.length() > 0) {
            Serial.println("Connecting to backend with API Key...");
        }
    });
    
    provisioner.startProvisioning();
}

void loop() {
    provisioner.loop();
}
