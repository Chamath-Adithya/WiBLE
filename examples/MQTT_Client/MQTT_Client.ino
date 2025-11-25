/**
 * MQTT_Client.ino
 * 
 * Demonstrates a full IoT workflow:
 * 1. Provision WiFi via BLE
 * 2. Connect to WiFi
 * 3. Connect to MQTT Broker
 * 4. Publish/Subscribe
 * 
 * Dependencies: PubSubClient by Nick O'Leary
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>
#include <WiFi.h>
#include <PubSubClient.h>

using namespace WiBLE;

// MQTT Configuration
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiBLE::WiBLE provisioner;
WiFiClient espClient;
PubSubClient client(espClient);

void onWiFiConnected(String ssid, String ip) {
    Serial.println("WiFi Connected! Connecting to MQTT...");
    // We can trigger MQTT connection here or in loop
}

void reconnectMQTT() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "WiBLEClient-";
        clientId += String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            client.publish("wible/status", "Online");
            client.subscribe("wible/control");
        } else {
            Serial.print("failed, rc=");
            // Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);
    
    // Setup WiBLE
    ProvisioningConfig config;
    config.deviceName = "WiBLE_MQTT_Node";
    
    provisioner.onWiFiConnected(onWiFiConnected);
    
    if (!provisioner.begin(config)) {
        Serial.println("WiBLE Init Failed");
        return;
    }
}

void loop() {
    // 1. Handle WiBLE (Provisioning & WiFi maintenance)
    provisioner.loop();
    
    // 2. Handle Application Logic (MQTT)
    // Only run MQTT if WiFi is connected
    if (provisioner.isWiFiConnected()) {
        if (!client.connected()) {
            reconnectMQTT();
        }
        client.loop();
        
        // Publish uptime every 10s
        static uint32_t lastMsg = 0;
        if (millis() - lastMsg > 10000) {
            lastMsg = millis();
            String msg = "Uptime: " + String(millis());
            client.publish("wible/uptime", msg.c_str());
        }
    }
}
