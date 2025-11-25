/**
 * ProvisioningOrchestrator.cpp - Orchestrator Implementation
 */

#include "ProvisioningOrchestrator.h"
#include "BLEManager.h"
#include "SecurityManager.h"
#include "StateManager.h"
#include "WiFiManager.h"
#include "utils/LogManager.h"

namespace WiBLE {

ProvisioningOrchestrator::ProvisioningOrchestrator(
    StateManager* stateMgr,
    BLEManager* bleMgr,
    WiFiManager* wifiMgr,
    SecurityManager* secMgr
) : stateManager(stateMgr), bleManager(bleMgr), wifiManager(wifiMgr), securityManager(secMgr) {
}

void ProvisioningOrchestrator::initialize() {
    // Register for BLE data events
    if (bleManager) {
        bleManager->onDataReceived([this](const String& uuid, const std::vector<uint8_t>& data) {
            processBLEData(uuid, data);
        });
    }
}

void ProvisioningOrchestrator::processBLEData(const String& characteristicUUID, const std::vector<uint8_t>& data) {
    if (characteristicUUID == WIBLE_CRED_CHARACTERISTIC) {
        handleCredentials(data);
    } else if (characteristicUUID == WIBLE_CONTROL_CHARACTERISTIC) {
        handleControlCommand(data);
    }
}

void ProvisioningOrchestrator::handleCredentials(const std::vector<uint8_t>& data) {
    LogManager::info("Received credentials packet (" + String((int)data.size()) + " bytes)");
    
    stateManager->handleEvent(StateEvent::CREDENTIALS_RECEIVED);
    
    // 1. Decrypt
    std::vector<uint8_t> decryptedData;
    if (securityManager && securityManager->isSessionEstablished()) {
        EncryptedMessage msg;
        // Assuming data format: [IV (16 bytes)] [Ciphertext]
        if (data.size() > 16) {
            msg.iv.assign(data.begin(), data.begin() + 16);
            msg.ciphertext.assign(data.begin() + 16, data.end());
            decryptedData = securityManager->decrypt(msg);
        } else {
            LogManager::error("Invalid encrypted packet size");
            return;
        }
    } else {
        // Plaintext (for testing or OPEN security)
        decryptedData = data;
    }
    
    if (decryptedData.empty()) {
        LogManager::error("Decryption failed or empty data");
        sendResponse("ERROR", "Decryption failed");
        return;
    }
    
    String jsonStr((char*)decryptedData.data(), decryptedData.size());
    LogManager::debug("Decrypted JSON: " + jsonStr);
    
    // 2. Parse
    WiFiCredentials creds = parseCredentials(jsonStr);
    if (!creds.isValid()) {
        LogManager::error("Invalid credentials format");
        sendResponse("ERROR", "Invalid format");
        return;
    }
    
    // 3. Connect
    stateManager->handleEvent(StateEvent::START_WIFI_CONNECT);
    if (wifiManager) {
        wifiManager->connect(creds.ssid, creds.password);
    }
}

void ProvisioningOrchestrator::handleControlCommand(const std::vector<uint8_t>& data) {
    // Handle commands like "SCAN", "RESET", etc.
}

WiFiCredentials ProvisioningOrchestrator::parseCredentials(const String& json) {
    WiFiCredentials creds;
    // Simple manual JSON parsing for "ssid":"..." and "pass":"..."
    // In production, use ArduinoJson
    
    int ssidStart = json.indexOf("\"ssid\":\"");
    if (ssidStart != -1) {
        ssidStart += 8;
        int ssidEnd = json.indexOf("\"", ssidStart);
        if (ssidEnd != -1) {
            creds.ssid = json.substring(ssidStart, ssidEnd);
        }
    }
    
    int passStart = json.indexOf("\"pass\":\"");
    if (passStart != -1) {
        passStart += 8;
        int passEnd = json.indexOf("\"", passStart);
        if (passEnd != -1) {
            creds.password = json.substring(passStart, passEnd);
        }
    }
    
    return creds;
}

void ProvisioningOrchestrator::sendResponse(const String& status, const String& message) {
    String response = "{\"status\":\"" + status + "\",\"msg\":\"" + message + "\"}";
    if (bleManager) {
        bleManager->notify(WIBLE_STATUS_CHARACTERISTIC, std::vector<uint8_t>(response.begin(), response.end()));
    }
}

void ProvisioningOrchestrator::onWiFiConnected(const ConnectionInfo& info) {
    stateManager->handleEvent(StateEvent::WIFI_CONNECTED);
    sendResponse("SUCCESS", "Connected to " + info.ssid);
}

void ProvisioningOrchestrator::onWiFiDisconnected(WiFiDisconnectReason reason) {
    stateManager->handleEvent(StateEvent::WIFI_DISCONNECT);
    sendResponse("ERROR", "WiFi Disconnected");
}

} // namespace WiBLE
