/**
 * DeepSleep.ino
 * 
 * Demonstrates how to use WiBLE with Deep Sleep.
 * 
 * Pattern:
 * 1. Wake up
 * 2. Check if provisioned
 * 3. If YES: Connect WiFi, Do Task, Sleep
 * 4. If NO: Start BLE Provisioning, Wait for user, Then Sleep
 */

#include <WiBLE.h>

using namespace WiBLE;

WiBLE::WiBLE provisioner;

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds) */

void setup() {
    Serial.begin(115200);
    delay(1000); // Allow serial to stabilize

    // Check wakeup reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    Serial.printf("Wakeup Reason: %d\n", wakeup_reason);

    ProvisioningConfig config;
    config.deviceName = "WiBLE_DeepSleep";
    
    // Initialize WiBLE (loads credentials)
    provisioner.begin(config);

    if (provisioner.isProvisioned()) {
        Serial.println("Device IS provisioned. Connecting to WiFi...");
        
        // Note: WiBLE auto-connects to WiFi if provisioned because autoReconnect=true by default
        // We just need to wait for connection
        
        uint32_t start = millis();
        while (!provisioner.isWiFiConnected() && millis() - start < 10000) {
            provisioner.loop();
            delay(10);
        }
        
        if (provisioner.isWiFiConnected()) {
            Serial.println("WiFi Connected! Performing Task...");
            // TODO: Send sensor data here
            delay(1000); // Simulate work
        } else {
            Serial.println("Failed to connect to WiFi.");
        }
        
        Serial.println("Going to sleep...");
        esp_deep_sleep_start();
        
    } else {
        Serial.println("Device NOT provisioned. Starting BLE Provisioning...");
        // Stay awake to allow user to provision
        // We will loop in the main loop()
    }
    
    // Configure sleep timer
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
    // Only reached if NOT provisioned
    provisioner.loop();
    
    // If we just got provisioned, we can go to sleep
    if (provisioner.isProvisioned() && provisioner.isWiFiConnected()) {
        Serial.println("Just provisioned! Going to sleep in 5 seconds...");
        delay(5000);
        esp_deep_sleep_start();
    }
}
