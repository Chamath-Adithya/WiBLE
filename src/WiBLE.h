/**
 * WiBLE - Your Gateway Between BLE and Wi-Fi
 * 
 * A production-grade, secure, and scalable BLE provisioning library
 * for ESP32/ESP32-C6 IoT devices.
 * 
 * @version 2.0.0
 * @author WiBLE Development Team
 * @license MIT
 */

#ifndef WIBLE_H
#define WIBLE_H

// ============================================================================
// INCLUDES
// ============================================================================
#include <Arduino.h>
#include <memory>
#include <vector>
#include <functional>
#include "WiBLE_Defs.h"

namespace WiBLE {

class BLEManager;
class WiFiManager;
class SecurityManager;
class StateManager;
class ProvisioningOrchestrator;
class StorageManager;
class LogManager;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct WiFiCredentials {
    String ssid;
    String password;
    String securityType = "WPA2";
    bool hidden = false;
    
    bool isValid() const {
        return !ssid.isEmpty() && ssid.length() <= 32 && 
               password.length() <= 64;
    }
};

struct DeviceInfo {
    String deviceName;
    String firmwareVersion;
    String hardwareVersion;
    String macAddress;
    uint32_t chipId;
    size_t freeHeap;
    size_t totalHeap;
};

struct ProvisioningConfig {
    // Device Configuration
    String deviceName = "WiBLE_Device";
    String manufacturerName = "WiBLE";
    String firmwareVersion = "2.0.0";
    
    // Security Configuration
    SecurityLevel securityLevel = SecurityLevel::SECURE;
    bool requirePinAuth = false;
    String pinCode = "000000";
    uint32_t authTimeoutMs = 30000;
    
    // BLE Configuration
    uint16_t mtuSize = 512;
    uint16_t connectionInterval = 24; // 30ms (24 * 1.25ms)
    uint16_t slaveLatency = 0;
    uint16_t supervisionTimeout = 400; // 4s
    bool enableBonding = true;
    
    // WiFi Configuration
    uint32_t wifiConnectTimeoutMs = 20000;
    uint8_t wifiMaxRetries = 3;
    uint32_t wifiRetryDelayMs = 2000;
    bool autoReconnect = true;
    bool persistCredentials = true;
    
    // Power Management
    bool enablePowerSaving = true;
    uint32_t bleScanWindowMs = 30;
    uint32_t bleScanIntervalMs = 100;
    
    // Logging
    LogLevel logLevel = LogLevel::INFO;
    bool enableSerialLog = true;
    bool enableFileLog = false;
    
    // Advanced Features
    bool enableOTA = false;
    bool enableTelemetry = false;
    uint16_t keepAliveIntervalS = 60;
    
    // Connection Management
    uint8_t maxSimultaneousConnections = 1;
    bool enableConnectionQueue = true;
};

struct ProvisioningMetrics {
    uint32_t totalProvisioningAttempts = 0;
    uint32_t successfulProvisionings = 0;
    uint32_t failedProvisionings = 0;
    uint32_t averageProvisioningTimeMs = 0;
    uint32_t totalConnectionAttempts = 0;
    uint32_t averageConnectionTimeMs = 0;
    uint32_t bleDisconnections = 0;
    uint32_t wifiDisconnections = 0;
    uint64_t uptimeSeconds = 0;
    size_t peakMemoryUsage = 0;
};

// ============================================================================
// CALLBACK TYPES
// ============================================================================

using StateChangeCallback = std::function<void(ProvisioningState oldState, ProvisioningState newState)>;
using BLEConnectedCallback = std::function<void(String clientAddress)>;
using BLEDisconnectedCallback = std::function<void(String clientAddress)>;
using AuthenticationCallback = std::function<void(bool success, String clientAddress)>;
using CredentialsReceivedCallback = std::function<void(const WiFiCredentials& credentials)>;
using WiFiConnectedCallback = std::function<void(String ssid, String ipAddress)>;
using WiFiDisconnectedCallback = std::function<void(String reason)>;
using ProvisioningCompleteCallback = std::function<void(bool success, uint32_t durationMs)>;
using ErrorCallback = std::function<void(ErrorCode error, String message, bool canRetry)>;
using ProgressCallback = std::function<void(uint8_t percentage, String message)>;
using DataReceivedCallback = std::function<void(const uint8_t* data, size_t length)>;

// ============================================================================
// RESULT TYPE FOR ERROR HANDLING
// ============================================================================

template<typename T>
class Result {
public:
    bool success;
    T value;
    ErrorCode errorCode;
    String errorMessage;
    
    Result(T val) : success(true), value(val), errorCode(ErrorCode::NONE) {}
    Result(ErrorCode code, String msg) : success(false), errorCode(code), errorMessage(msg) {}
    
    operator bool() const { return success; }
    T& operator*() { return value; }
    const T& operator*() const { return value; }
};

// ============================================================================
// MAIN WIBLE CLASS
// ============================================================================

class WiBLE {
public:
    // Constructor & Destructor
    WiBLE();
    ~WiBLE();
    
    // ========================================================================
    // LIFECYCLE MANAGEMENT
    // ========================================================================
    
    /**
     * Initialize the library with configuration
     * @param config Provisioning configuration
     * @return true if initialization successful
     */
    bool begin(const ProvisioningConfig& config = ProvisioningConfig());
    
    /**
     * Main loop - must be called regularly
     */
    void loop();
    
    /**
     * Stop all services and cleanup
     */
    void end();
    
    /**
     * Reset to factory defaults
     */
    void reset();
    
    // ========================================================================
    // STATE MANAGEMENT
    // ========================================================================
    
    ProvisioningState getState() const;
    bool isProvisioned() const;
    bool isBLEConnected() const;
    bool isWiFiConnected() const;
    
    // ========================================================================
    // PROVISIONING CONTROL
    // ========================================================================
    
    /**
     * Start BLE advertising for provisioning
     */
    bool startProvisioning();
    
    /**
     * Stop provisioning and advertising
     */
    void stopProvisioning();
    
    /**
     * Manually provision with credentials
     */
    Result<bool> provisionManually(const WiFiCredentials& credentials);
    
    /**
     * Clear stored credentials and reset provisioning
     */
    void clearProvisioning();
    
    // ========================================================================
    // WIFI MANAGEMENT
    // ========================================================================
    
    /**
     * Get list of available WiFi networks
     */
    std::vector<String> scanWiFiNetworks(bool showHidden = false);
    
    /**
     * Connect to WiFi with credentials
     */
    Result<bool> connectWiFi(const WiFiCredentials& credentials);
    
    /**
     * Disconnect from WiFi
     */
    void disconnectWiFi();
    
    /**
     * Get WiFi connection status
     */
    String getWiFiStatus() const;
    
    /**
     * Get current IP address
     */
    String getIPAddress() const;
    
    /**
     * Get signal strength (RSSI)
     */
    int8_t getWiFiRSSI() const;
    
    // ========================================================================
    // SECURITY
    // ========================================================================
    
    /**
     * Set custom encryption key (advanced users)
     */
    bool setEncryptionKey(const uint8_t* key, size_t length);
    
    /**
     * Enable/disable secure pairing
     */
    void setSecureMode(bool enabled);
    
    /**
     * Get security status
     */
    bool isSecureConnectionEstablished() const;
    
    // ========================================================================
    // DEVICE INFORMATION
    // ========================================================================
    
    DeviceInfo getDeviceInfo() const;
    ProvisioningMetrics getMetrics() const;
    WiFiCredentials getStoredCredentials() const;
    
    // ========================================================================
    // DATA TRANSFER
    // ========================================================================
    
    /**
     * Send custom data over BLE
     */
    Result<bool> sendBLEData(const uint8_t* data, size_t length);
    
    /**
     * Send custom data over WiFi
     */
    Result<bool> sendWiFiData(const String& endpoint, const String& data);
    
    // ========================================================================
    // CALLBACK REGISTRATION
    // ========================================================================
    
    void onStateChange(StateChangeCallback callback);
    void onBLEConnected(BLEConnectedCallback callback);
    void onBLEDisconnected(BLEDisconnectedCallback callback);
    void onAuthentication(AuthenticationCallback callback);
    void onCredentialsReceived(CredentialsReceivedCallback callback);
    void onWiFiConnected(WiFiConnectedCallback callback);
    void onWiFiDisconnected(WiFiDisconnectedCallback callback);
    void onProvisioningComplete(ProvisioningCompleteCallback callback);
    void onError(ErrorCallback callback);
    void onProgress(ProgressCallback callback);
    void onDataReceived(DataReceivedCallback callback);
    
    // ========================================================================
    // LOGGING & DEBUGGING
    // ========================================================================
    
    void setLogLevel(LogLevel level);
    void enableSerialLogging(bool enabled);
    void log(LogLevel level, const String& message);
    void dumpState() const;
    
    // ========================================================================
    // ADVANCED FEATURES
    // ========================================================================
    
    /**
     * Enable OTA updates
     */
    bool enableOTA(const String& otaUrl = "");
    
    /**
     * Send telemetry data
     */
    void sendTelemetry(const String& data);
    
    /**
     * Set custom provisioning data
     */
    void setCustomData(const String& key, const String& value);
    String getCustomData(const String& key) const;

private:
    // Core Components (using PIMPL pattern for cleaner API)
    std::unique_ptr<BLEManager> bleManager;
    std::unique_ptr<WiFiManager> wifiManager;
    std::unique_ptr<SecurityManager> securityManager;
    std::unique_ptr<StateManager> stateManager;
    std::unique_ptr<ProvisioningOrchestrator> orchestrator;
    std::unique_ptr<StorageManager> storageManager;
    std::unique_ptr<LogManager> logManager;
    
    // Configuration
    ProvisioningConfig config;
    
    // Callbacks
    StateChangeCallback stateChangeCallback;
    BLEConnectedCallback bleConnectedCallback;
    BLEDisconnectedCallback bleDisconnectedCallback;
    AuthenticationCallback authenticationCallback;
    CredentialsReceivedCallback credentialsReceivedCallback;
    WiFiConnectedCallback wifiConnectedCallback;
    WiFiDisconnectedCallback wifiDisconnectedCallback;
    ProvisioningCompleteCallback provisioningCompleteCallback;
    ErrorCallback errorCallback;
    ProgressCallback progressCallback;
    DataReceivedCallback dataReceivedCallback;
    
    // Internal state
    bool initialized;
    uint32_t startTime;
    ProvisioningMetrics metrics;
    
    // Internal methods
    void initializeComponents();
    void handleStateTransition(ProvisioningState newState);
    void handleError(ErrorCode code, const String& message, bool canRetry = false);
    void updateMetrics();
    void triggerCallbackSafely(std::function<void()> callback);
};

} // namespace WiBLE

#endif // WIBLE_H
