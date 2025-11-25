/**
 * WiFiManager.h - WiFi Management for WiBLE
 * 
 * Handles WiFi scanning, connection, reconnection,
 * and network management with retry logic.
 */

#ifndef WIBLE_WIFI_MANAGER_H
#define WIBLE_WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <functional>
#include <vector>
#include <map>

namespace WiBLE {

// ============================================================================
// WIFI ENUMS
// ============================================================================

enum class WiFiConnectionState {
    DISCONNECTED,
    SCANNING,
    CONNECTING,
    CONNECTED,
    CONNECTION_FAILED,
    CONNECTION_LOST
};

enum class WiFiSecurityType {
    OPEN,
    WEP,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK,
    WPA2_ENTERPRISE,
    WPA3_PSK
};

enum class WiFiDisconnectReason {
    UNKNOWN,
    USER_REQUESTED,
    CONNECTION_TIMEOUT,
    AUTHENTICATION_FAILED,
    SSID_NOT_FOUND,
    WEAK_SIGNAL,
    AP_DISCONNECTED,
    DHCP_FAILED
};

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

struct WiFiConfig {
    // Connection parameters
    uint32_t connectionTimeoutMs = 20000;
    uint8_t maxConnectionRetries = 3;
    uint32_t retryDelayMs = 2000;
    bool useExponentialBackoff = true;
    
    // Auto-reconnect
    bool autoReconnect = true;
    uint32_t reconnectIntervalMs = 5000;
    uint8_t maxReconnectAttempts = 10;
    
    // Scanning
    bool scanHiddenNetworks = true;
    uint8_t maxScanResults = 20;
    uint32_t scanTimeoutMs = 5000;
    
    // Power management
    bool enablePowerSaving = false;
    wifi_ps_type_t powerSaveMode = WIFI_PS_MIN_MODEM;
    
    // IP Configuration
    bool useDHCP = true;
    String staticIP = "";
    String gateway = "";
    String subnet = "255.255.255.0";
    String dns1 = "";
    String dns2 = "";
    
    // Advanced
    int8_t minRSSI = -80;  // Minimum acceptable signal strength
    uint8_t channel = 0;   // 0 = auto
    bool fastConnect = false;  // Skip scan if SSID/channel known
    uint16_t keepAliveIntervalS = 60;
};

// ============================================================================
// NETWORK INFO
// ============================================================================

struct NetworkInfo {
    String ssid;
    int8_t rssi;
    uint8_t channel;
    WiFiSecurityType securityType;
    bool isHidden;
    uint8_t* bssid;
    
    NetworkInfo() : rssi(0), channel(0), 
                   securityType(WiFiSecurityType::OPEN),
                   isHidden(false), bssid(nullptr) {}
    
    String getSecurityString() const;
    uint8_t getSignalQuality() const;  // 0-100%
};

struct ConnectionInfo {
    String ssid;
    String ipAddress;
    String gateway;
    String subnet;
    String macAddress;
    int8_t rssi;
    uint8_t channel;
    uint32_t connectedAt;
    uint32_t connectionDuration;
    
    ConnectionInfo() : rssi(0), channel(0), 
                      connectedAt(0), connectionDuration(0) {}
};

// ============================================================================
// CONNECTION RESULT
// ============================================================================

struct ConnectionResult {
    bool success;
    WiFiConnectionState state;
    WiFiDisconnectReason failureReason;
    String errorMessage;
    uint32_t connectionTimeMs;
    uint8_t attemptCount;
    
    ConnectionResult() : success(false), 
                        state(WiFiConnectionState::DISCONNECTED),
                        failureReason(WiFiDisconnectReason::UNKNOWN),
                        connectionTimeMs(0), attemptCount(0) {}
};

// ============================================================================
// CALLBACKS
// ============================================================================

using WiFiMgrConnectedCallback = std::function<void(const ConnectionInfo& info)>;
using WiFiMgrDisconnectedCallback = std::function<void(WiFiDisconnectReason reason, String message)>;
using WiFiScanCompleteCallback = std::function<void(const std::vector<NetworkInfo>& networks)>;
using WiFiIPAcquiredCallback = std::function<void(String ipAddress)>;
using WiFiConnectionProgressCallback = std::function<void(uint8_t progress, String status)>;

// ============================================================================
// WIFI MANAGER CLASS
// ============================================================================

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * Initialize WiFi subsystem
     */
    bool initialize(const WiFiConfig& config = WiFiConfig());
    
    /**
     * Cleanup WiFi resources
     */
    void cleanup();
    
    /**
     * Reset WiFi module
     */
    void reset();
    
    /**
     * Check if initialized
     */
    bool isInitialized() const { return initialized; }
    
    // ========================================================================
    // NETWORK SCANNING
    // ========================================================================
    
    /**
     * Scan for available networks (blocking)
     */
    std::vector<NetworkInfo> scanNetworks(bool showHidden = false);
    
    /**
     * Scan for available networks (non-blocking)
     */
    void scanNetworksAsync(WiFiScanCompleteCallback callback);
    
    /**
     * Check if specific SSID is available
     */
    bool isNetworkAvailable(const String& ssid);
    
    /**
     * Get network info by SSID
     */
    NetworkInfo getNetworkInfo(const String& ssid);
    
    /**
     * Get strongest network
     */
    NetworkInfo getStrongestNetwork(const std::vector<String>& ssidList);
    
    // ========================================================================
    // CONNECTION MANAGEMENT
    // ========================================================================
    
    /**
     * Connect to WiFi network
     */
    ConnectionResult connect(const String& ssid, const String& password,
                            WiFiSecurityType securityType = WiFiSecurityType::WPA2_PSK);
    
    /**
     * Connect with retry logic
     */
    ConnectionResult connectWithRetry(const String& ssid, const String& password);
    
    /**
     * Disconnect from current network
     */
    void disconnect();
    
    /**
     * Check if connected
     */
    bool isConnected() const;
    
    /**
     * Get current connection state
     */
    WiFiConnectionState getConnectionState() const;
    
    /**
     * Get connection info
     */
    ConnectionInfo getConnectionInfo() const;
    
    // ========================================================================
    // AUTO-RECONNECT
    // ========================================================================
    
    /**
     * Enable/disable auto-reconnect
     */
    void setAutoReconnect(bool enabled);
    
    /**
     * Check auto-reconnect status
     */
    bool isAutoReconnectEnabled() const;
    
    /**
     * Handle reconnection logic (call in loop)
     */
    void handleReconnection();
    
    // ========================================================================
    // STORED CREDENTIALS
    // ========================================================================
    
    /**
     * Save credentials to persistent storage
     */
    bool saveCredentials(const String& ssid, const String& password);
    
    /**
     * Load credentials from storage
     */
    bool loadCredentials(String& ssid, String& password);
    
    /**
     * Clear saved credentials
     */
    void clearCredentials();
    
    /**
     * Check if credentials are saved
     */
    bool hasStoredCredentials() const;
    
    /**
     * Connect using stored credentials
     */
    ConnectionResult connectWithStoredCredentials();
    
    // ========================================================================
    // NETWORK INFORMATION
    // ========================================================================
    
    /**
     * Get current SSID
     */
    String getSSID() const;
    
    /**
     * Get IP address
     */
    String getIPAddress() const;
    
    /**
     * Get gateway
     */
    String getGateway() const;
    
    /**
     * Get subnet mask
     */
    String getSubnetMask() const;
    
    /**
     * Get DNS servers
     */
    std::vector<String> getDNSServers() const;
    
    /**
     * Get MAC address
     */
    String getMACAddress() const;
    
    /**
     * Get RSSI (signal strength)
     */
    int8_t getRSSI() const;
    
    /**
     * Get channel
     */
    uint8_t getChannel() const;
    
    /**
     * Get signal quality (0-100%)
     */
    uint8_t getSignalQuality() const;
    
    // ========================================================================
    // STATIC IP CONFIGURATION
    // ========================================================================
    
    /**
     * Configure static IP
     */
    bool configureStaticIP(const String& ip, const String& gateway, 
                          const String& subnet, const String& dns1 = "",
                          const String& dns2 = "");
    
    /**
     * Switch to DHCP
     */
    void enableDHCP();
    
    /**
     * Check if using DHCP
     */
    bool isDHCPEnabled() const;
    
    // ========================================================================
    // POWER MANAGEMENT
    // ========================================================================
    
    /**
     * Enable/disable power saving mode
     */
    void setPowerSaveMode(bool enabled);
    
    /**
     * Set WiFi transmit power
     */
    void setTxPower(wifi_power_t power);
    
    /**
     * Get current power mode
     */
    wifi_ps_type_t getPowerSaveMode() const;
    
    // ========================================================================
    // CONNECTIVITY TESTING
    // ========================================================================
    
    /**
     * Ping gateway
     */
    bool pingGateway(uint32_t timeoutMs = 1000);
    
    /**
     * Ping specific host
     */
    bool pingHost(const String& host, uint32_t timeoutMs = 1000);
    
    /**
     * Test internet connectivity
     */
    bool hasInternetAccess(uint32_t timeoutMs = 5000);
    
    /**
     * Get connection quality score (0-100)
     */
    uint8_t getConnectionQuality();
    
    // ========================================================================
    // MULTI-NETWORK SUPPORT
    // ========================================================================
    
    /**
     * Add network to priority list
     */
    void addNetwork(const String& ssid, const String& password, uint8_t priority = 0);
    
    /**
     * Remove network from list
     */
    void removeNetwork(const String& ssid);
    
    /**
     * Connect to best available network from list
     */
    ConnectionResult connectToBestNetwork();
    
    /**
     * Get network list
     */
    std::vector<String> getNetworkList() const;
    
    // ========================================================================
    // CALLBACKS
    // ========================================================================
    
    void onConnected(WiFiMgrConnectedCallback callback);
    void onDisconnected(WiFiMgrDisconnectedCallback callback);
    void onScanComplete(WiFiScanCompleteCallback callback);
    void onIPAcquired(WiFiIPAcquiredCallback callback);
    void onConnectionProgress(WiFiConnectionProgressCallback callback);
    
    // ========================================================================
    // MONITORING
    // ========================================================================
    
    /**
     * Start connection monitoring (call in loop)
     */
    void monitor();
    
    /**
     * Get connection statistics
     */
    struct ConnectionStats {
        uint32_t totalConnections;
        uint32_t successfulConnections;
        uint32_t failedConnections;
        uint32_t totalReconnects;
        uint32_t averageConnectionTimeMs;
        uint32_t totalUptime;
        uint32_t totalDowntime;
        uint32_t longestConnection;
    };
    
    ConnectionStats getStatistics() const;
    
    // ========================================================================
    // DEBUGGING
    // ========================================================================
    
    void dumpNetworkInfo() const;
    void dumpConnectionState() const;
    void dumpStatistics() const;
    String getStatusString() const;

private:
    // Configuration
    WiFiConfig config;
    
    // Connection state
    WiFiConnectionState connectionState;
    String currentSSID;
    String currentPassword;
    bool initialized;
    
    // Auto-reconnect state
    bool autoReconnectEnabled;
    uint32_t lastReconnectAttempt;
    uint8_t reconnectAttemptCount;
    
    // Multi-network support
    struct NetworkEntry {
        String ssid;
        String password;
        uint8_t priority;
    };
    std::vector<NetworkEntry> knownNetworks;
    
    // Statistics
    ConnectionStats statistics;
    uint32_t connectionStartTime;
    uint32_t lastConnectionTime;
    
    // Callbacks
    WiFiConnectedCallback connectedCallback;
    WiFiDisconnectedCallback disconnectedCallback;
    WiFiScanCompleteCallback scanCompleteCallback;
    WiFiIPAcquiredCallback ipAcquiredCallback;
    WiFiConnectionProgressCallback progressCallback;
    
    // Scan state
    bool scanInProgress;
    std::vector<NetworkInfo> lastScanResults;
    
    // Internal methods
    bool connectInternal(const String& ssid, const String& password, 
                        uint8_t attemptNumber = 1);
    void handleConnectionSuccess();
    void handleConnectionFailure(WiFiDisconnectReason reason);
    void updateConnectionState(WiFiConnectionState newState);
    uint32_t calculateRetryDelay(uint8_t attemptNumber);
    WiFiDisconnectReason getDisconnectReason();
    void updateStatistics();
    void notifyProgress(uint8_t progress, const String& status);
    NetworkInfo convertWiFiScanResult(int index);
    WiFiSecurityType getSecurityType(wifi_auth_mode_t authMode);
    static void WiFiEventHandler(WiFiEvent_t event);
    
    // Static instance for event handler
    static WiFiManager* instance;
};

// ============================================================================
// WIFI UTILITIES
// ============================================================================

class WiFiUtils {
public:
    /**
     * Validate SSID format
     */
    static bool isValidSSID(const String& ssid);
    
    /**
     * Validate password strength
     */
    static bool isValidPassword(const String& password, WiFiSecurityType securityType);
    
    /**
     * Convert RSSI to quality percentage
     */
    static uint8_t rssiToQuality(int8_t rssi);
    
    /**
     * Convert quality to signal bars (1-5)
     */
    static uint8_t qualityToBars(uint8_t quality);
    
    /**
     * Get security type string
     */
    static String securityTypeToString(WiFiSecurityType type);
    
    /**
     * Parse security type from string
     */
    static WiFiSecurityType stringToSecurityType(const String& str);
    
    /**
     * Get disconnect reason string
     */
    static String disconnectReasonToString(WiFiDisconnectReason reason);
    
    /**
     * Check if IP address is valid
     */
    static bool isValidIPAddress(const String& ip);
    
    /**
     * Compare WiFi security levels
     */
    static bool isMoreSecure(WiFiSecurityType a, WiFiSecurityType b);
};

} // namespace WiBLE

#endif // WIBLE_WIFI_MANAGER_H
