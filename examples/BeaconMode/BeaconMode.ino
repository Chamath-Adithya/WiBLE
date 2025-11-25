/**
 * BeaconMode.ino
 * 
 * Demonstrates the "Beacon Mode" capability of WiBLE.
 * The ESP32 acts as an iBeacon when not in provisioning mode.
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// Beacon Configuration
// UUID: 12345678-1234-1234-1234-1234567890ab
String BEACON_UUID = "12345678-1234-1234-1234-1234567890ab";
uint16_t BEACON_MAJOR = 1;
uint16_t BEACON_MINOR = 100;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting WiBLE Beacon Mode...");

    // Configure WiBLE
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Beacon";
    
    // Initialize
    if (provisioner.begin(config)) {
        Serial.println("WiBLE Initialized!");
    } else {
        Serial.println("Initialization Failed!");
        return;
    }

    // Start Beacon Mode immediately
    Serial.println("Starting iBeacon...");
    provisioner.startBeaconMode(BEACON_UUID, BEACON_MAJOR, BEACON_MINOR);
    Serial.println("Beacon is active. Use a Beacon Scanner app to find it.");
}

void loop() {
    provisioner.loop();
    
    // You can switch back to provisioning mode if a button is pressed, for example.
    // if (buttonPressed) {
    //     provisioner.startProvisioning();
    // }
}
