/**
 * BasicProvisioning.ino
 * 
 * Complete example demonstrating WiBLE provisioning
 * with all major features: BLE connection, secure authentication,
 * WiFi provisioning, error handling, and callbacks.
 * 
 * Hardware: ESP32 / ESP32-C6
 * Board: ESP32 Dev Module
 * 
 * Setup:
 * 1. Flash this code to ESP32
 * 2. Use WiBLE mobile app to provision
 * 3. Device connects to WiFi automatically
 * 4. Monitor Serial for logs
 * 
 * RECOMMENDATION:
 * This is the recommended starting point for production applications.
 * It includes robust error handling, logging, and a CLI.
 * @author Chamath Adithya (SOLVEO)
 */

#include <Arduino.h>
#include <Arduino.h>
#include "WiBLE.h"
#include "StateManager.h" // For StateUtils

using namespace WiBLE;

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

WiBLE::WiBLE provisioner;

// ============================================================================
// CONFIGURATION
// ============================================================================

void setupConfiguration() {
    ProvisioningConfig config;
    
    // Device Identity
    config.deviceName = "WiBLE_ESP32";
    config.manufacturerName = "YourCompany";
    config.firmwareVersion = "2.0.0";
    
    // Security Settings
    config.securityLevel = SecurityLevel::SECURE;  // ECDH + AES-256
    config.requirePinAuth = false;                 // Optional PIN
    config.pinCode = "123456";
    
    // BLE Settings
    config.mtuSize = 512;                          // Maximum MTU
    config.connectionInterval = 24;                // 30ms for fast transfer
    config.enableBonding = true;
    
    // WiFi Settings
    config.wifiConnectTimeoutMs = 20000;          // 20s timeout
    config.wifiMaxRetries = 3;
    config.wifiRetryDelayMs = 2000;
    config.autoReconnect = true;                  // Auto-reconnect if disconnected
    config.persistCredentials = true;             // Save to NVS
    
    // Power Management
    config.enablePowerSaving = false;             // Keep active during provisioning
    
    // Logging
    config.logLevel = LogLevel::INFO;
    config.enableSerialLog = true;
    
    // Initialize library
    if (provisioner.begin(config)) {
        Serial.println("✓ WiBLE initialized successfully");
    } else {
        Serial.println("✗ WiBLE initialization failed!");
    }
}

// ============================================================================
// CALLBACK HANDLERS
// ============================================================================

void onStateChange(ProvisioningState oldState, ProvisioningState newState) {
    Serial.printf("State: %s → %s\n", 
                  StateUtils::stateToString(oldState).c_str(),
                  StateUtils::stateToString(newState).c_str());
    
    // Update LED or display based on state
    switch (newState) {
        case ProvisioningState::BLE_ADVERTISING:
            // Blink LED to indicate advertising
            break;
        case ProvisioningState::BLE_CONNECTED:
            // Solid LED to indicate connection
            break;
        case ProvisioningState::PROVISIONED:
            // Green LED for success
            break;
        case ProvisioningState::ERROR:
            // Red LED for error
            break;
        default:
            break;
    }
}

void onBLEConnected(String clientAddress) {
    Serial.printf("📱 Mobile app connected: %s\n", clientAddress.c_str());
    Serial.println("   Waiting for credentials...");
}

void onBLEDisconnected(String clientAddress) {
    Serial.printf("📱 Mobile app disconnected: %s\n", clientAddress.c_str());
}

void onAuthentication(bool success, String clientAddress) {
    if (success) {
        Serial.printf("🔐 Authentication successful with %s\n", clientAddress.c_str());
        Serial.println("   Secure channel established (AES-256)");
    } else {
        Serial.printf("🔐 Authentication failed with %s\n", clientAddress.c_str());
    }
}

void onCredentialsReceived(const WiFiCredentials& credentials) {
    Serial.println("📥 WiFi credentials received:");
    Serial.printf("   SSID: %s\n", credentials.ssid.c_str());
    String maskedPass = "";
    for(int i=0; i<credentials.password.length(); i++) maskedPass += "*";
    Serial.printf("   Password: %s\n", maskedPass.c_str());
    Serial.printf("   Security: %s\n", credentials.securityType.c_str());
}

void onWiFiConnected(String ssid, String ipAddress) {
    Serial.println("✓ WiFi connected successfully!");
    Serial.printf("   SSID: %s\n", ssid.c_str());
    Serial.printf("   IP Address: %s\n", ipAddress.c_str());
    Serial.printf("   Signal: %d dBm\n", provisioner.getWiFiRSSI());
}

void onWiFiDisconnected(String reason) {
    Serial.printf("✗ WiFi disconnected: %s\n", reason.c_str());
    
    // if (provisioner.isAutoReconnectEnabled()) {
    //     Serial.println("   Auto-reconnect enabled, will retry...");
    // }
}

void onProvisioningComplete(bool success, uint32_t durationMs) {
    if (success) {
        Serial.println("\n╔═══════════════════════════════════════╗");
        Serial.println("║   PROVISIONING COMPLETED SUCCESSFULLY  ║");
        Serial.println("╚═══════════════════════════════════════╝");
        Serial.printf("Total time: %.2f seconds\n\n", durationMs / 1000.0);
        
        // Print connection info
        auto info = provisioner.getDeviceInfo();
        Serial.println("Device Information:");
        Serial.printf("  • Device Name: %s\n", info.deviceName.c_str());
        Serial.printf("  • MAC Address: %s\n", info.macAddress.c_str());
        Serial.printf("  • IP Address: %s\n", provisioner.getIPAddress().c_str());
        Serial.printf("  • Firmware: %s\n", info.firmwareVersion.c_str());
        Serial.printf("  • Free Heap: %d KB\n", info.freeHeap / 1024);
        
        // Device is now ready for normal operation
        // You can now connect to cloud, start sensors, etc.
        
    } else {
        Serial.println("\n╔═══════════════════════════════════════╗");
        Serial.println("║      PROVISIONING FAILED              ║");
        Serial.println("╚═══════════════════════════════════════╝");
        Serial.printf("Duration: %.2f seconds\n\n", durationMs / 1000.0);
    }
}

void onError(ErrorCode error, String message, bool canRetry) {
    Serial.println("\n⚠️  ERROR OCCURRED");
    Serial.printf("   Code: %d\n", static_cast<int>(error));
    Serial.printf("   Message: %s\n", message.c_str());
    Serial.printf("   Can Retry: %s\n", canRetry ? "Yes" : "No");
    
    // Handle specific errors
    switch (error) {
        case ErrorCode::BLE_CONNECTION_LOST:
            Serial.println("   → Mobile app disconnected unexpectedly");
            break;
            
        case ErrorCode::WIFI_CONNECTION_FAILED:
            Serial.println("   → Check WiFi credentials and signal strength");
            break;
            
        case ErrorCode::AUTHENTICATION_FAILED:
            Serial.println("   → Security handshake failed, try again");
            break;
            
        case ErrorCode::TIMEOUT_ERROR:
            Serial.println("   → Operation took too long, retrying...");
            break;
            
        default:
            Serial.println("   → Unexpected error occurred");
            break;
    }
    
    if (canRetry) {
        Serial.println("   Library will automatically retry...");
    } else {
        Serial.println("   Manual intervention required");
    }
}

void onProgress(uint8_t percentage, String message) {
    // Create progress bar
    Serial.printf("\r[");
    int barWidth = 30;
    int pos = barWidth * percentage / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) Serial.print("█");
        else if (i == pos) Serial.print(">");
        else Serial.print(" ");
    }
    Serial.printf("] %3d%% - %s   ", percentage, message.c_str());
    
    if (percentage == 100) {
        Serial.println();
    }
}

void onDataReceived(const uint8_t* data, size_t length) {
    Serial.printf("📨 Custom data received: %d bytes\n", length);
    // Handle custom data from mobile app if needed
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("╔═══════════════════════════════════════════════════╗");
    Serial.println("║         WiBLE Provisioning Library v2.0          ║");
    Serial.println("║    Advanced BLE + WiFi Provisioning System       ║");
    Serial.println("╚═══════════════════════════════════════════════════╝");
    Serial.println();
    
    // Configure library
    setupConfiguration();
    
    // Register callbacks
    Serial.println("Registering callbacks...");
    provisioner.onStateChange(onStateChange);
    provisioner.onBLEConnected(onBLEConnected);
    provisioner.onBLEDisconnected(onBLEDisconnected);
    provisioner.onAuthentication(onAuthentication);
    provisioner.onCredentialsReceived(onCredentialsReceived);
    provisioner.onWiFiConnected(onWiFiConnected);
    provisioner.onWiFiDisconnected(onWiFiDisconnected);
    provisioner.onProvisioningComplete(onProvisioningComplete);
    provisioner.onError(onError);
    provisioner.onProgress(onProgress);
    provisioner.onDataReceived(onDataReceived);
    
    Serial.println("✓ Callbacks registered\n");
    
    // Check if already provisioned
    if (provisioner.isProvisioned()) {
        Serial.println("Device already provisioned!");
        Serial.println("Attempting to connect with stored credentials...\n");
        
        auto credentials = provisioner.getStoredCredentials();
        Serial.printf("Stored SSID: %s\n", credentials.ssid.c_str());
        
        auto result = provisioner.provisionManually(credentials);
        if (result.success) {
            Serial.println("✓ Connected with stored credentials");
        } else {
            Serial.println("✗ Stored credentials invalid, clearing...");
            provisioner.clearProvisioning();
        }
    }
    
    // Start provisioning if not connected
    if (!provisioner.isWiFiConnected()) {
        Serial.println("Starting BLE provisioning mode...");
        Serial.println("Open WiBLE mobile app and scan for devices\n");
        
        if (provisioner.startProvisioning()) {
            Serial.println("✓ BLE advertising started");
            Serial.printf("   Device Name: %s\n", provisioner.getDeviceInfo().deviceName.c_str());
            Serial.printf("   MAC Address: %s\n", provisioner.getDeviceInfo().macAddress.c_str());
        } else {
            Serial.println("✗ Failed to start provisioning!");
        }
    }
    
    Serial.println("\n==================================================");
    Serial.println("Setup complete. Entering main loop...");
    Serial.println("==================================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // CRITICAL: Call this every loop iteration
    // Handles state machine, BLE operations, WiFi monitoring
    provisioner.loop();
    
    // Your application code here
    // Only runs after provisioning is complete
    if (provisioner.isProvisioned() && provisioner.isWiFiConnected()) {
        
        // Example: Send sensor data to cloud
        static uint32_t lastTelemetry = 0;
        if (millis() - lastTelemetry > 60000) { // Every minute
            lastTelemetry = millis();
            
            // Collect sensor data
            String telemetry = "{\"temp\":25.5,\"humidity\":60}";
            
            // Send to cloud (implement your own cloud client)
            // provisioner.sendTelemetry(telemetry);
            
            Serial.printf("📊 Telemetry sent: %s\n", telemetry.c_str());
        }
        
        // Example: Check connection quality
        static uint32_t lastQualityCheck = 0;
        if (millis() - lastQualityCheck > 10000) { // Every 10s
            lastQualityCheck = millis();
            
            int8_t rssi = provisioner.getWiFiRSSI();
            String quality = rssi > -50 ? "Excellent" :
                           rssi > -60 ? "Good" :
                           rssi > -70 ? "Fair" : "Poor";
            
            Serial.printf("📶 WiFi Signal: %d dBm (%s)\n", rssi, quality.c_str());
        }
    }
    
    // Debug: Print metrics every 5 minutes
    static uint32_t lastMetrics = 0;
    if (millis() - lastMetrics > 300000) {
        lastMetrics = millis();
        
        auto metrics = provisioner.getMetrics();
        Serial.println("\n📈 Provisioning Metrics:");
        Serial.printf("   Total Attempts: %d\n", metrics.totalProvisioningAttempts);
        Serial.printf("   Success Rate: %.1f%%\n", 
                     (float)metrics.successfulProvisionings / metrics.totalProvisioningAttempts * 100);
        Serial.printf("   Avg Duration: %d ms\n", metrics.averageProvisioningTimeMs);
        Serial.printf("   Uptime: %d hours\n\n", metrics.uptimeSeconds / 3600);
    }
    
    // Don't overwhelm the CPU
    delay(10);
}

// ============================================================================
// OPTIONAL: COMMAND LINE INTERFACE
// ============================================================================

void handleSerialCommand() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "status") {
            provisioner.dumpState();
        }
        else if (command == "reset") {
            Serial.println("Resetting device...");
            provisioner.clearProvisioning();
            ESP.restart();
        }
        else if (command == "scan") {
            Serial.println("Scanning WiFi networks...");
            auto networks = provisioner.scanWiFiNetworks();
            for (const auto& network : networks) {
                Serial.printf("  • %s (%d dBm)\n", network.c_str(), -50);
            }
        }
        else if (command == "metrics") {
            auto metrics = provisioner.getMetrics();
            Serial.println("\nMetrics:");
            Serial.printf("  Success Rate: %.1f%%\n", 
                         (float)metrics.successfulProvisionings / 
                         metrics.totalProvisioningAttempts * 100);
            Serial.printf("  Avg Time: %d ms\n", metrics.averageProvisioningTimeMs);
        }
        else if (command == "help") {
            Serial.println("\nAvailable Commands:");
            Serial.println("  status  - Show current state");
            Serial.println("  reset   - Clear provisioning and restart");
            Serial.println("  scan    - Scan WiFi networks");
            Serial.println("  metrics - Show provisioning metrics");
            Serial.println("  help    - Show this message");
        }
        else {
            Serial.println("Unknown command. Type 'help' for available commands.");
        }
    }
}

// Add to loop() if you want CLI support:
// handleSerialCommand();
