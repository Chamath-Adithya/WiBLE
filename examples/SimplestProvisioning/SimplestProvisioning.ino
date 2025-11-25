/**
 * SimplestProvisioning.ino
 * 
 * The absolute minimal example - just 20 lines!
 * Perfect for quick testing or embedding in tight spaces.
 * 
 * For production use, see BasicProvisioning.ino instead.
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;
WiBLE provisioner;

void setup() {
    Serial.begin(115200);
    
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Simple";
    
    provisioner.begin(config);
    provisioner.startProvisioning();
    
    Serial.println("Ready! Use BLE app to provision.");
}

void loop() {
    provisioner.loop();
}
