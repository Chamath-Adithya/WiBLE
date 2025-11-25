/**
 * OTA_Update.ino
 * 
 * Demonstrates Complex Scenario: Firmware Update via BLE.
 * 
 * Scenario:
 * 1. Device advertises "OTA Service".
 * 2. Mobile app connects and writes firmware chunks to "OTA Characteristic".
 * 3. Device writes chunks to OTA partition using `Update` library.
 */

#include <WiBLE.h>
#include <Update.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// In a real implementation, we would add a specific OTA Service to BLEManager.
// For this example, we'll reuse the "Custom Data" channel of WiBLE to receive firmware chunks.
// Protocol: 
// - First packet: "START:<Size>"
// - Subsequent packets: Binary Data
// - Last packet: "END"

bool otaStarted = false;
size_t totalSize = 0;
size_t receivedSize = 0;

void onDataReceived(const uint8_t* data, size_t len) {
    String packet = "";
    for(size_t i=0; i<min(len, (size_t)20); i++) packet += (char)data[i];
    
    if (packet.startsWith("START:")) {
        totalSize = packet.substring(6).toInt();
        Serial.printf("OTA Start. Size: %d\n", totalSize);
        if (Update.begin(totalSize)) {
            otaStarted = true;
            receivedSize = 0;
        } else {
            Serial.println("OTA Begin Failed");
        }
        return;
    }
    
    if (otaStarted) {
        if (packet.startsWith("END")) {
            if (Update.end()) {
                Serial.println("OTA Success! Rebooting...");
                delay(1000);
                ESP.restart();
            } else {
                Serial.printf("OTA Failed. Error: %d\n", Update.getError());
            }
            otaStarted = false;
        } else {
            // Write chunk
            size_t written = Update.write((uint8_t*)data, len);
            receivedSize += written;
            Serial.printf("Progress: %d / %d\n", receivedSize, totalSize);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting OTA Example...");

    ProvisioningConfig config;
    config.deviceName = "WiBLE_OTA_Target";
    
    provisioner.onDataReceived(onDataReceived);
    provisioner.begin(config);
}

void loop() {
    provisioner.loop();
}
