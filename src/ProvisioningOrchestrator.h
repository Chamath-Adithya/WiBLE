/**
 * ProvisioningOrchestrator.h - Coordinates provisioning flow
 */

#ifndef PROVISIONING_ORCHESTRATOR_H
#define PROVISIONING_ORCHESTRATOR_H

#include <Arduino.h>
#include <memory>
#include "WiBLE_Defs.h"
#include "WiFiManager.h" // For WiFiCredentials

namespace WiBLE {

class BLEManager;
class SecurityManager;
class StateManager;
class WiFiManager;

class ProvisioningOrchestrator {
public:
    ProvisioningOrchestrator(
        StateManager* stateMgr,
        BLEManager* bleMgr,
        WiFiManager* wifiMgr,
        SecurityManager* secMgr
    );
    
    void initialize();
    
    // Handle incoming data from BLE
    void processBLEData(const String& characteristicUUID, const std::vector<uint8_t>& data);
    
    // Handle WiFi events
    void onWiFiConnected(const ConnectionInfo& info);
    void onWiFiDisconnected(WiFiDisconnectReason reason);

private:
    StateManager* stateManager;
    BLEManager* bleManager;
    WiFiManager* wifiManager;
    SecurityManager* securityManager;
    
    void handleCredentials(const std::vector<uint8_t>& data);
    void handleControlCommand(const std::vector<uint8_t>& data);
    
    // Helper to parse JSON credentials (simple parser)
    WiFiCredentials parseCredentials(const String& json);
    
    void sendResponse(const String& status, const String& message);
};

} // namespace WiBLE

#endif
