/**
 * WebClient.ino
 * 
 * Firmware counterpart for the Web Bluetooth Provisioning Client.
 * 
 * Instructions:
 * 1. Flash this sketch to your ESP32.
 * 2. Open the 'index.html' file in this directory using a Chrome/Edge browser.
 * 3. Click "Connect" in the web page to provision this device.
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

void onProvisioningComplete(bool success, uint32_t duration) {
    if (success) {
        Serial.printf("Provisioning Successful! Took %d ms\n", duration);
        // The Web Client will receive a notification via the Status Characteristic
    } else {
        Serial.println("Provisioning Failed.");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Web Client Target...");

    ProvisioningConfig config;
    config.deviceName = "WiBLE_Web_Target";
    
    // The Web Client expects standard WiBLE UUIDs, which are default.
    
    provisioner.onProvisioningComplete(onProvisioningComplete);
    provisioner.begin(config);
    
    Serial.println("Ready for Web Bluetooth connection.");
}

void loop() {
    provisioner.loop();
}
