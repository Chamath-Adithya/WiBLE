/**
 * WiBLE.cpp - Main WiBLE Class Implementation
 */

#include "WiBLE.h"
#include "BLEManager.h"
#include "WiFiManager.h"
#include "SecurityManager.h"
#include "StateManager.h"
#include "ProvisioningOrchestrator.h"
#include "utils/LogManager.h"

namespace WiBLE {

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================================

WiBLE::WiBLE() : initialized(false), startTime(0) {
    // Initialize PIMPL pointers
    // Note: In a full implementation, we would initialize all managers here.
    // For Phase 1, we focus on StateManager.
    stateManager = std::unique_ptr<StateManager>(new StateManager());
    
    // Initialize Managers
    bleManager = std::unique_ptr<BLEManager>(new BLEManager());
    wifiManager = std::unique_ptr<WiFiManager>(new WiFiManager());
    securityManager = std::unique_ptr<SecurityManager>(new SecurityManager());
    
    // Initialize Orchestrator with references to managers
    orchestrator = std::unique_ptr<ProvisioningOrchestrator>(
        new ProvisioningOrchestrator(
            stateManager.get(),
            bleManager.get(),
            wifiManager.get(),
            securityManager.get()
        )
    );
}

WiBLE::~WiBLE() {
    end();
}

// ============================================================================
// LIFECYCLE MANAGEMENT
// ============================================================================

bool WiBLE::begin(const ProvisioningConfig& config) {
    this->config = config;
    
    // Initialize Logging
    if (config.enableSerialLog) {
        // Serial.begin(115200); // User usually does this in setup()
        LogManager::info("WiBLE initializing...");
    }
    
    // Initialize State Manager
    if (stateManager) {
        stateManager->initialize();
        
        // Register internal state callbacks
        stateManager->onStateTransition([this](ProvisioningState from, ProvisioningState to, StateEvent event) {
            handleStateTransition(to);
        });
    }
    
    // Initialize other components
    if (bleManager) {
        BLEConfig bleConfig;
        bleConfig.deviceName = config.deviceName;
        bleConfig.mtuSize = config.mtuSize;
        bleConfig.connectionInterval = config.connectionInterval;
        bleConfig.enableBonding = config.enableBonding;
        bleManager->initialize(bleConfig);
    }
    
    if (securityManager) {
        SecurityConfig secConfig;
        secConfig.level = config.securityLevel;
        secConfig.pinCode = config.pinCode;
        secConfig.authTimeoutMs = config.authTimeoutMs;
        securityManager->initialize(secConfig);
    }
    
    if (wifiManager) {
        WiFiConfig wifiConfig;
        wifiConfig.connectionTimeoutMs = config.wifiConnectTimeoutMs;
        wifiConfig.maxConnectionRetries = config.wifiMaxRetries;
        wifiConfig.retryDelayMs = config.wifiRetryDelayMs;
        wifiConfig.autoReconnect = config.autoReconnect;
        wifiConfig.persistCredentials = config.persistCredentials;
        wifiManager->initialize(wifiConfig);
    }
    if (orchestrator) orchestrator->initialize();
    
    initialized = true;
    startTime = millis();
    
    LogManager::info("WiBLE initialized successfully");
    return true;
}

void WiBLE::loop() {
    if (!initialized) return;
    
    // 1. Update State Machine
    // stateManager->checkTimeouts(); // TODO: Implement checkTimeouts
    
    // 2. Process BLE Operations
    // if (bleManager) bleManager->processOperationQueue();
    
    // 3. Monitor WiFi
    // if (wifiManager) wifiManager->monitor();
}

void WiBLE::end() {
    initialized = false;
    // Cleanup resources
    LogManager::info("WiBLE stopped");
}

// ============================================================================
// STATE MANAGEMENT
// ============================================================================

ProvisioningState WiBLE::getState() const {
    return stateManager ? stateManager->getCurrentState() : ProvisioningState::IDLE;
}

bool WiBLE::isProvisioned() const {
    return getState() == ProvisioningState::PROVISIONED;
}

bool WiBLE::isBLEConnected() const {
    return getState() == ProvisioningState::BLE_CONNECTED ||
           getState() == ProvisioningState::AUTHENTICATING ||
           getState() == ProvisioningState::RECEIVING_CREDENTIALS;
}

bool WiBLE::isWiFiConnected() const {
    // return wifiManager && wifiManager->isConnected();
    return false; // Placeholder
}

// ============================================================================
// PROVISIONING CONTROL
// ============================================================================

bool WiBLE::startProvisioning() {
    if (!initialized) return false;
    
    LogManager::info("Starting provisioning...");
    
    // Trigger state transition
    if (stateManager) {
        return stateManager->handleEvent(StateEvent::START_ADVERTISING);
    }
    
    return false;
}

void WiBLE::stopProvisioning() {
    if (stateManager) {
        stateManager->handleEvent(StateEvent::STOP_ADVERTISING);
    }
}

Result<bool> WiBLE::provisionManually(const WiFiCredentials& credentials) {
    if (!credentials.isValid()) {
        return Result<bool>(ErrorCode::WIFI_CREDENTIALS_INVALID, "Invalid credentials");
    }
    
    // TODO: Implement manual provisioning logic
    // 1. Set credentials
    // 2. Trigger WiFi connect
    
    return Result<bool>(true);
}

void WiBLE::clearProvisioning() {
    // TODO: Clear credentials from storage
    if (stateManager) {
        stateManager->reset();
    }
}

// ============================================================================
// CALLBACK REGISTRATION
// ============================================================================

void WiBLE::onStateChange(StateChangeCallback callback) {
    stateChangeCallback = callback;
}

void WiBLE::onBLEConnected(BLEConnectedCallback callback) {
    bleConnectedCallback = callback;
}

void WiBLE::onBLEDisconnected(BLEDisconnectedCallback callback) {
    bleDisconnectedCallback = callback;
}

void WiBLE::onAuthentication(AuthenticationCallback callback) {
    authenticationCallback = callback;
}

void WiBLE::onCredentialsReceived(CredentialsReceivedCallback callback) {
    credentialsReceivedCallback = callback;
}

void WiBLE::onWiFiConnected(WiFiConnectedCallback callback) {
    wifiConnectedCallback = callback;
}

void WiBLE::onWiFiDisconnected(WiFiDisconnectedCallback callback) {
    wifiDisconnectedCallback = callback;
}

void WiBLE::onProvisioningComplete(ProvisioningCompleteCallback callback) {
    provisioningCompleteCallback = callback;
}

void WiBLE::onError(ErrorCallback callback) {
    errorCallback = callback;
}

void WiBLE::onProgress(ProgressCallback callback) {
    progressCallback = callback;
}

void WiBLE::onDataReceived(DataReceivedCallback callback) {
    dataReceivedCallback = callback;
}

// ============================================================================
// INTERNAL METHODS
// ============================================================================

void WiBLE::handleStateTransition(ProvisioningState newState) {
    // Notify user callback
    if (stateChangeCallback) {
        // Note: We need previous state here, but for now just passing IDLE as dummy
        // In real impl, StateManager passes both
        stateChangeCallback(ProvisioningState::IDLE, newState);
    }

    // Broadcast new state via BLE Advertising
    if (bleManager && bleManager->isInitialized()) {
        uint8_t statusByte = 0x00;
        switch (newState) {
            case ProvisioningState::IDLE: statusByte = 0x00; break;
            case ProvisioningState::CONNECTING_WIFI: statusByte = 0x01; break;
            case ProvisioningState::PROVISIONED: statusByte = 0x02; break;
            case ProvisioningState::ERROR: statusByte = 0x03; break;
            default: statusByte = 0xFF; break; // Other states
        }
        
        // Company ID 0xFFFF (Test), Data: [Status]
        if (statusByte != 0xFF) {
            bleManager->setManufacturerData(0xFFFF, &statusByte, 1);
        }
    }
    
    // Handle specific state actions
    switch (newState) {
        case ProvisioningState::PROVISIONED:
            if (provisioningCompleteCallback) {
                provisioningCompleteCallback(true, millis() - startTime);
            }
            break;
            
        case ProvisioningState::ERROR:
            if (errorCallback) {
                errorCallback(ErrorCode::UNKNOWN_ERROR, "State machine entered error state", false);
            }
            break;
            
        default:
            break;
    }
}

// ============================================================================
// PLACEHOLDER METHODS (To satisfy header)
// ============================================================================

std::vector<String> WiBLE::scanWiFiNetworks(bool showHidden) { return {}; }
Result<bool> WiBLE::connectWiFi(const WiFiCredentials& credentials) { return Result<bool>(false); }
void WiBLE::disconnectWiFi() {}
String WiBLE::getWiFiStatus() const { return "Disconnected"; }
String WiBLE::getIPAddress() const { return "0.0.0.0"; }
int8_t WiBLE::getWiFiRSSI() const { return 0; }
bool WiBLE::setEncryptionKey(const uint8_t* key, size_t length) { return false; }
void WiBLE::setSecureMode(bool enabled) {}
bool WiBLE::isSecureConnectionEstablished() const { return false; }
DeviceInfo WiBLE::getDeviceInfo() const { return DeviceInfo(); }
ProvisioningMetrics WiBLE::getMetrics() const { return metrics; }
WiFiCredentials WiBLE::getStoredCredentials() const { return WiFiCredentials(); }
Result<bool> WiBLE::sendBLEData(const uint8_t* data, size_t length) { return Result<bool>(false); }
Result<bool> WiBLE::sendWiFiData(const String& endpoint, const String& data) { return Result<bool>(false); }
void WiBLE::setLogLevel(LogLevel level) {}
void WiBLE::enableSerialLogging(bool enabled) {}
void WiBLE::log(LogLevel level, const String& message) { LogManager::log(level, message); }
void WiBLE::dumpState() const { if (stateManager) stateManager->dumpStateMachine(); }
bool WiBLE::enableOTA(const ::String& otaUrl) { return false; }
void WiBLE::sendTelemetry(const ::String& data) {}
void WiBLE::setCustomData(const ::String& key, const ::String& value) {}
::String WiBLE::getCustomData(const ::String& key) const { return ""; }

} // namespace WiBLE
