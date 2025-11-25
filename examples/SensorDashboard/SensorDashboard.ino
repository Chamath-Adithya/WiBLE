/**
 * SensorDashboard.ino
 * 
 * Demonstrates bidirectional data transfer in a real IoT scenario:
 * - Reads 4 sensors and sends data to mobile app
 * - Receives 2 LED control signals from mobile app
 * 
 * Perfect example for building IoT dashboards, smart home devices,
 * or any project requiring sensor monitoring + remote control.
 * 
 * Hardware Setup:
 * - Connect sensors to analog pins (or simulate)
 * - LED1 on GPIO 2
 * - LED2 on GPIO 4
 * 
 * @author Chamath Adithya (SOLVEO)
 */

#include <WiBLE.h>

using namespace WiBLE;
WiBLE provisioner;

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// LED Pins
const int LED1_PIN = 2;
const int LED2_PIN = 4;

// Sensor Pins (using analog pins - can be replaced with I2C/SPI sensors)
const int TEMP_SENSOR_PIN = 34;     // Simulated temperature
const int HUMIDITY_SENSOR_PIN = 35; // Simulated humidity
const int LIGHT_SENSOR_PIN = 36;    // LDR or light sensor
const int MOTION_SENSOR_PIN = 39;   // PIR or motion sensor

// ============================================================================
// SENSOR DATA STRUCTURE
// ============================================================================

struct SensorData {
    float temperature;  // °C
    float humidity;     // %
    int light;          // 0-4095 (12-bit ADC)
    bool motion;        // true/false
};

SensorData currentSensors;

// ============================================================================
// LED STATE
// ============================================================================

bool led1State = false;
bool led2State = false;

// ============================================================================
// TIMING
// ============================================================================

unsigned long lastSensorUpdate = 0;
const unsigned long SENSOR_UPDATE_INTERVAL = 2000; // Send sensor data every 2 seconds

// ============================================================================
// PROTOCOL DEFINITIONS
// ============================================================================

// Commands from App → Device
#define CMD_LED1_CONTROL  0x01
#define CMD_LED2_CONTROL  0x02

// Commands from Device → App
#define CMD_SENSOR_DATA   0x10

// ============================================================================
// SENSOR READING FUNCTIONS
// ============================================================================

void readSensors() {
    // Read analog values (0-4095 on ESP32 12-bit ADC)
    int tempRaw = analogRead(TEMP_SENSOR_PIN);
    int humidityRaw = analogRead(HUMIDITY_SENSOR_PIN);
    
    // Convert to real values (example conversion - adjust for your sensors)
    // For demo, we'll simulate realistic values
    currentSensors.temperature = 20.0 + (tempRaw / 4095.0) * 15.0; // 20-35°C
    currentSensors.humidity = 30.0 + (humidityRaw / 4095.0) * 50.0; // 30-80%
    currentSensors.light = analogRead(LIGHT_SENSOR_PIN);
    currentSensors.motion = digitalRead(MOTION_SENSOR_PIN) == HIGH;
    
    // Debug output
    Serial.printf("📊 Sensors: Temp=%.1f°C, Humidity=%.1f%%, Light=%d, Motion=%s\n",
                  currentSensors.temperature,
                  currentSensors.humidity,
                  currentSensors.light,
                  currentSensors.motion ? "YES" : "NO");
}

// ============================================================================
// DATA TRANSMISSION
// ============================================================================

void sendSensorDataToApp() {
    // Build packet: [CMD_ID][Temperature(4)][Humidity(4)][Light(2)][Motion(1)]
    uint8_t packet[12];
    packet[0] = CMD_SENSOR_DATA;
    
    // Pack temperature (float to bytes)
    memcpy(&packet[1], &currentSensors.temperature, 4);
    
    // Pack humidity
    memcpy(&packet[5], &currentSensors.humidity, 4);
    
    // Pack light (uint16_t)
    uint16_t lightValue = (uint16_t)currentSensors.light;
    memcpy(&packet[9], &lightValue, 2);
    
    // Pack motion (1 byte)
    packet[11] = currentSensors.motion ? 1 : 0;
    
    // Send via BLE
    if (provisioner.sendBLEData(packet, sizeof(packet))) {
        Serial.println("✅ Sensor data sent to app");
    } else {
        Serial.println("❌ Failed to send sensor data");
    }
}

// ============================================================================
// LED CONTROL
// ============================================================================

void setLED1(bool state) {
    led1State = state;
    digitalWrite(LED1_PIN, state ? HIGH : LOW);
    Serial.printf("💡 LED1: %s\n", state ? "ON" : "OFF");
}

void setLED2(bool state) {
    led2State = state;
    digitalWrite(LED2_PIN, state ? HIGH : LOW);
    Serial.printf("💡 LED2: %s\n", state ? "ON" : "OFF");
}

// ============================================================================
// RECEIVE COMMANDS FROM APP
// ============================================================================

void onDataReceived(const uint8_t* data, size_t len) {
    if (len < 2) {
        Serial.println("⚠️ Invalid command (too short)");
        return;
    }
    
    uint8_t command = data[0];
    uint8_t value = data[1];
    
    switch (command) {
        case CMD_LED1_CONTROL:
            setLED1(value != 0);
            Serial.printf("📱 App → LED1: %s\n", value ? "ON" : "OFF");
            break;
            
        case CMD_LED2_CONTROL:
            setLED2(value != 0);
            Serial.printf("📱 App → LED2: %s\n", value ? "ON" : "OFF");
            break;
            
        default:
            Serial.printf("⚠️ Unknown command: 0x%02X\n", command);
            break;
    }
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== WiBLE Sensor Dashboard ===");
    
    // Configure LED pins
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    setLED1(false);
    setLED2(false);
    
    // Configure sensor pins
    pinMode(MOTION_SENSOR_PIN, INPUT);
    // Analog pins (34-39) don't need pinMode configuration
    
    // Configure WiBLE
    ProvisioningConfig config;
    config.deviceName = "WiBLE_Dashboard";
    config.securityLevel = SecurityLevel::SECURE;
    
    provisioner.begin(config);
    
    // Register callbacks
    provisioner.onBLEConnected([](String address) {
        Serial.printf("📱 Mobile app connected: %s\n", address.c_str());
    });
    
    provisioner.onBLEDisconnected([](String address) {
        Serial.println("📱 Mobile app disconnected");
        // Turn off LEDs when app disconnects
        setLED1(false);
        setLED2(false);
    });
    
    provisioner.onDataReceived(onDataReceived);
    
    provisioner.onWiFiConnected([](String ssid, String ip) {
        Serial.printf("✅ WiFi Connected: %s (IP: %s)\n", ssid.c_str(), ip.c_str());
    });
    
    // Start provisioning
    provisioner.startProvisioning();
    
    Serial.println("✅ Dashboard ready!");
    Serial.println("📌 Connect via BLE app to:");
    Serial.println("   - View real-time sensor data");
    Serial.println("   - Control LED1 and LED2");
    Serial.println("\n📡 Protocol:");
    Serial.println("   App → Device: [CMD][VALUE]");
    Serial.println("     0x01 = LED1 (0=OFF, 1=ON)");
    Serial.println("     0x02 = LED2 (0=OFF, 1=ON)");
    Serial.println("   Device → App: [0x10][Temp][Hum][Light][Motion]");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    provisioner.loop();
    
    // Send sensor data periodically (only if BLE connected)
    if (millis() - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
        lastSensorUpdate = millis();
        
        readSensors();
        
        // Only send if BLE is connected
        if (provisioner.isBLEConnected()) {
            sendSensorDataToApp();
        }
    }
}
