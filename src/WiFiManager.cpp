/**
 * WiFiManager.cpp - WiFi Management Implementation
 */

#include "WiFiManager.h"
#include "utils/LogManager.h"
#include <Preferences.h>

namespace WiBLE {

// ============================================================================
// WIFI MANAGER IMPLEMENTATION
// ============================================================================

WiFiManager::WiFiManager() 
    : initialized(false), 
      isScanning(false) {
}

WiFiManager::~WiFiManager() {
    disconnect();
}

bool WiFiManager::initialize(const WiFiConfig& config) {
    this->config = config;
    
    // Set mode to STATION (client)
    WiFi.mode(WIFI_STA);
    
    // Configure auto-reconnect
    WiFi.setAutoReconnect(config.autoReconnect);
    
    // Set static IP if configured
    if (!config.staticIP.isEmpty()) {
        configureStaticIP(config.staticIP, config.gateway, config.subnet, config.dns1, config.dns2);
    }
    
    initialized = true;
    LogManager::info("WiFiManager initialized");
    return true;
}

// ============================================================================
// SCANNING
// ============================================================================

void WiFiManager::scanNetworksAsync(WiFiScanCompleteCallback callback) {
    if (isScanning) return;
    
    scanCompleteCallback = callback;
    isScanning = true;
    LogManager::info("Starting WiFi scan...");
    
    // Async scan
    WiFi.scanNetworks(true);
}

std::vector<NetworkInfo> WiFiManager::scanNetworks(bool showHidden) {
    LogManager::info("Starting blocking WiFi scan...");
    int16_t count = WiFi.scanNetworks(false); // Blocking
    
    std::vector<NetworkInfo> networks;
    
    if (count == -2) {
        // Scan not triggered
        return networks;
    } else if (count == -1) {
        // Scan in progress
        return networks;
    }
    
    for (int i = 0; i < count; ++i) {
        NetworkInfo info;
        info.ssid = WiFi.SSID(i);
        info.rssi = WiFi.RSSI(i);
        info.channel = WiFi.channel(i);
        
        // Map encryption type
        switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN: info.securityType = WiFiSecurityType::OPEN; break;
            case WIFI_AUTH_WEP: info.securityType = WiFiSecurityType::WEP; break;
            case WIFI_AUTH_WPA_PSK: info.securityType = WiFiSecurityType::WPA_PSK; break;
            case WIFI_AUTH_WPA2_PSK: info.securityType = WiFiSecurityType::WPA2_PSK; break;
            case WIFI_AUTH_WPA_WPA2_PSK: info.securityType = WiFiSecurityType::WPA_WPA2_PSK; break;
            case WIFI_AUTH_WPA2_ENTERPRISE: info.securityType = WiFiSecurityType::WPA2_ENTERPRISE; break;
            default: info.securityType = WiFiSecurityType::OPEN; break; // Fallback
        }
        
        networks.push_back(info);
    }
    
    WiFi.scanDelete();
    isScanning = false;
    
    if (scanCompleteCallback) {
        scanCompleteCallback(networks);
    }
    
    return networks;
}

// ============================================================================
// CONNECTION
// ============================================================================

ConnectionResult WiFiManager::connect(const String& ssid, const String& password, WiFiSecurityType securityType) {
    ConnectionResult result;
    
    LogManager::info("Connecting to WiFi: " + ssid);
    
    if (ssid.isEmpty()) {
        result.success = false;
        result.errorMessage = "Empty SSID";
        return result;
    }
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection (blocking for simplicity in this method, but usually handled async in loop)
    // For this implementation, we'll return immediately and rely on callbacks/loop monitoring
    // unless we want a blocking connect.
    // Let's implement a simple blocking wait with timeout for the `connect` method,
    // as it returns a Result.
    
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < config.connectionTimeoutMs) {
        delay(100);
        if (progressCallback) {
            // Calculate progress roughly
            uint8_t progress = (millis() - start) * 100 / config.connectionTimeoutMs;
            progressCallback(progress, "Connecting...");
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        result.success = true;
        result.state = WiFiConnectionState::CONNECTED;
        result.connectionTimeMs = millis() - start;
        LogManager::info("WiFi Connected! IP: " + WiFi.localIP().toString());
        
        if (connectedCallback) {
            connectedCallback(getConnectionInfo());
        }
        
        // Save credentials if configured
        if (config.persistCredentials) {
            saveCredentials(ssid, password);
        }
        
    } else {
        result.success = false;
        result.state = WiFiConnectionState::DISCONNECTED;
        result.errorMessage = "Timeout";
        LogManager::error("WiFi Connection Failed");
        
        if (disconnectedCallback) {
            disconnectedCallback(WiFiDisconnectReason::UNKNOWN, "Connection timeout");
        }
    }
    
    return result;
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);
    LogManager::info("WiFi Disconnected");
}

// ============================================================================
// CREDENTIALS STORAGE
// ============================================================================

bool WiFiManager::saveCredentials(const String& ssid, const String& password) {
    Preferences prefs;
    if (!prefs.begin("wible_creds", false)) return false;
    
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    prefs.end();
    return true;
}

bool WiFiManager::loadCredentials(String& ssid, String& password) {
    Preferences prefs;
    if (!prefs.begin("wible_creds", true)) return false;
    
    ssid = prefs.getString("ssid", "");
    password = prefs.getString("pass", "");
    prefs.end();
    
    return !ssid.isEmpty();
}

void WiFiManager::clearCredentials() {
    Preferences prefs;
    if (prefs.begin("wible_creds", false)) {
        prefs.clear();
        prefs.end();
    }
}

// ============================================================================
// INFO & STATUS
// ============================================================================

ConnectionInfo WiFiManager::getConnectionInfo() const {
    ConnectionInfo info;
    if (WiFi.status() == WL_CONNECTED) {
        info.ssid = WiFi.SSID();
        info.ipAddress = WiFi.localIP().toString();
        info.gateway = WiFi.gatewayIP().toString();
        info.subnet = WiFi.subnetMask().toString();
        info.macAddress = WiFi.macAddress();
        info.rssi = WiFi.RSSI();
    }
    return info;
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIPAddress() const {
    return WiFi.localIP().toString();
}

// ============================================================================
// CALLBACK REGISTRATION
// ============================================================================

void WiFiManager::onConnected(WiFiMgrConnectedCallback callback) {
    connectedCallback = callback;
}

void WiFiManager::onDisconnected(WiFiMgrDisconnectedCallback callback) {
    disconnectedCallback = callback;
}

void WiFiManager::onScanComplete(WiFiScanCompleteCallback callback) {
    scanCompleteCallback = callback;
}

void WiFiManager::onConnectionProgress(WiFiConnectionProgressCallback callback) {
    progressCallback = callback;
}

// ============================================================================
// PLACEHOLDERS
// ============================================================================

void WiFiManager::monitor() {
    // Check connection status periodically
    // Handle auto-reconnect if needed
}

bool WiFiManager::configureStaticIP(const String& ip, const String& gateway, const String& subnet, const String& dns1, const String& dns2) {
    // IPAddress local_ip, gw, sn, d1, d2;
    // local_ip.fromString(ip); ...
    // return WiFi.config(local_ip, gw, sn, d1, d2);
    return true; 
}

} // namespace WiBLE
