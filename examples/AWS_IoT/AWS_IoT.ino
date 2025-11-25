/**
 * AWS_IoT.ino
 * 
 * Demonstrates Complex Scenario: Secure Cloud Connectivity.
 * 
 * Scenario:
 * 1. Provision WiFi via WiBLE.
 * 2. Receive AWS Certificates (Root CA, Cert, Key) via WiBLE Custom Data.
 * 3. Connect to AWS IoT Core using MQTTS.
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;
WiFiClientSecure net;
PubSubClient client(net);

// AWS Configuration (Usually hardcoded or stored in SPIFFS)
// Here we demonstrate receiving them via BLE
String AWS_CERT_CA = "";
String AWS_CERT_CRT = "";
String AWS_CERT_PRIVATE = "";
String AWS_IOT_ENDPOINT = "example.iot.us-east-1.amazonaws.com";

bool awsConfigured = false;

void onDataReceived(const uint8_t* data, size_t len) {
    // Simple protocol: First byte = type (1=CA, 2=CRT, 3=Key, 4=Endpoint)
    if (len < 2) return;
    
    uint8_t type = data[0];
    String content = "";
    for(size_t i=1; i<len; i++) content += (char)data[i];
    
    switch(type) {
        case 1: 
            AWS_CERT_CA = content; 
            Serial.println("Received CA Cert");
            break;
        case 2: 
            AWS_CERT_CRT = content; 
            Serial.println("Received Client Cert");
            break;
        case 3: 
            AWS_CERT_PRIVATE = content; 
            Serial.println("Received Private Key");
            break;
        case 4:
            AWS_IOT_ENDPOINT = content;
            Serial.println("Received Endpoint");
            break;
    }
    
    if (AWS_CERT_CA.length() > 0 && AWS_CERT_CRT.length() > 0 && AWS_CERT_PRIVATE.length() > 0) {
        awsConfigured = true;
        Serial.println("AWS Configuration Complete!");
        
        // Configure Network
        net.setCACert(AWS_CERT_CA.c_str());
        net.setCertificate(AWS_CERT_CRT.c_str());
        net.setPrivateKey(AWS_CERT_PRIVATE.c_str());
        client.setServer(AWS_IOT_ENDPOINT.c_str(), 8883);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting AWS IoT Example...");

    ProvisioningConfig config;
    config.deviceName = "WiBLE_AWS_Node";
    
    provisioner.onDataReceived(onDataReceived);
    provisioner.begin(config);
}

void loop() {
    provisioner.loop();
    
    if (provisioner.isWiFiConnected() && awsConfigured) {
        if (!client.connected()) {
            Serial.print("Connecting to AWS IoT...");
            if (client.connect("WiBLE_Thing")) {
                Serial.println("Connected!");
                client.subscribe("esp32/sub");
            } else {
                Serial.print("Failed. State: ");
                // Serial.println(client.state());
                delay(2000);
            }
        } else {
            client.loop();
        }
    }
}
