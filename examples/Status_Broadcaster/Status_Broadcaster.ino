/**
 * Status_Broadcaster.ino
 * 
 * Demonstrates Method 3: Advertising (Broadcasting).
 * 
 * Scenario:
 * The device broadcasts sensor data (e.g., Temperature, Humidity) 
 * in the Manufacturer Data field of the advertising packet.
 * No connection is required to read this data.
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

// Fake Sensor Data
struct SensorData {
    uint8_t type;       // 0x01 = Env Data
    uint16_t temp;      // Temperature * 100
    uint16_t humidity;  // Humidity * 100
    uint32_t counter;   // Packet counter
} __attribute__((packed));

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Status Broadcaster...");

    ProvisioningConfig config;
    config.deviceName = "WiBLE_Sensor";
    
    if (!provisioner.begin(config)) {
        Serial.println("Init Failed");
        return;
    }
    
    Serial.println("Broadcasting initialized.");
}

void loop() {
    provisioner.loop();
    
    static uint32_t lastUpdate = 0;
    static uint32_t counter = 0;
    
    // Update broadcast data every 2 seconds
    if (millis() - lastUpdate > 2000) {
        SensorData data;
        data.type = 0x01;
        data.temp = 2450 + random(-50, 50); // 24.50 C +/- 0.5
        data.humidity = 6000 + random(-100, 100); // 60.00% +/- 1.0
        data.counter = counter++;
        
        Serial.printf("Broadcasting: Temp=%.2f C, Hum=%.2f %%, Cnt=%d\n", 
                      data.temp/100.0, data.humidity/100.0, data.counter);
        
        // Broadcast with Company ID 0xFFFF (Testing)
        provisioner.startBroadcasting(0xFFFF, (uint8_t*)&data, sizeof(SensorData));
        
        lastUpdate = millis();
    }
}
