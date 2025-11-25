/**
 * WiBLE_Defs.h - Common definitions for WiBLE
 */

#ifndef WIBLE_DEFS_H
#define WIBLE_DEFS_H

#include <Arduino.h>

namespace WiBLE {

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

enum class ProvisioningState {
    IDLE,
    BLE_ADVERTISING,
    BLE_CONNECTED,
    AUTHENTICATING,
    RECEIVING_CREDENTIALS,
    CONNECTING_WIFI,
    VALIDATING_CONNECTION,
    PROVISIONED,
    ERROR
};

enum class SecurityLevel {
    NONE,           // No encryption (dev only)
    BASIC,          // Simple pairing
    SECURE,         // ECDH + AES-128
    ENTERPRISE      // ECDH + AES-256 + Certificate pinning
};

enum class ErrorCode {
    NONE = 0,
    BLE_INIT_FAILED,
    BLE_CONNECTION_LOST,
    WIFI_INIT_FAILED,
    WIFI_CONNECTION_FAILED,
    WIFI_CREDENTIALS_INVALID,
    AUTHENTICATION_FAILED,
    ENCRYPTION_FAILED,
    STORAGE_FAILED,
    TIMEOUT_ERROR,
    UNKNOWN_ERROR
};

enum class LogLevel {
    VERBOSE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    NONE
};

} // namespace WiBLE

#endif // WIBLE_DEFS_H
