/**
 * BLEManager.cpp - BLE Management Implementation
 */

#include "BLEManager.h"
#include "utils/LogManager.h"

namespace WiBLE {

// ============================================================================
// BLE MANAGER IMPLEMENTATION
// ============================================================================

BLEManager::BLEManager() 
    : initialized(false), 
      advertisingActive(false),
      bleServer(nullptr),
      provisioningService(nullptr),
      deviceInfoService(nullptr),
      advertising(nullptr) {
}

BLEManager::~BLEManager() {
    cleanup();
}

bool BLEManager::initialize(const BLEConfig& config) {
    this->config = config;
    
    // Initialize BLE Device
    BLEDevice::init(config.deviceName.c_str());
    
    // Create Server
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new ServerCallbacks(this));
    
    // Initialize Services
    if (!initializeServices()) {
        LogManager::error("Failed to initialize BLE services");
        return false;
    }
    
    // Setup Advertising
    advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(WIBLE_SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06); // functions that help with iPhone connections
    advertising->setMinPreferred(0x12);
    
    initialized = true;
    LogManager::info("BLEManager initialized");
    return true;
}

void BLEManager::cleanup() {
    if (advertisingActive) {
        stopAdvertising();
    }
    // Note: ESP32 BLE library doesn't have a full de-init, usually we just stop advertising
    initialized = false;
}

bool BLEManager::initializeServices() {
    // 1. Create Provisioning Service
    provisioningService = bleServer->createService(WIBLE_SERVICE_UUID);
    
    // Credentials Characteristic (Write)
    credentialsChar = provisioningService->createCharacteristic(
        WIBLE_CRED_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_WRITE
    );
    credentialsChar->setCallbacks(new CharacteristicCallbacks(this, WIBLE_CRED_CHARACTERISTIC));
    
    // Status Characteristic (Notify)
    statusChar = provisioningService->createCharacteristic(
        WIBLE_STATUS_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    statusChar->addDescriptor(new BLE2902());
    
    // Control Characteristic (Write)
    controlChar = provisioningService->createCharacteristic(
        WIBLE_CONTROL_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_WRITE
    );
    controlChar->setCallbacks(new CharacteristicCallbacks(this, WIBLE_CONTROL_CHARACTERISTIC));
    
    // Data Characteristic (Read/Write/Notify)
    dataChar = provisioningService->createCharacteristic(
        WIBLE_DATA_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_WRITE | 
        BLECharacteristic::PROPERTY_NOTIFY
    );
    dataChar->setCallbacks(new CharacteristicCallbacks(this, WIBLE_DATA_CHARACTERISTIC));
    dataChar->addDescriptor(new BLE2902());
    
    provisioningService->start();
    
    // 2. Create Device Info Service (Optional but recommended)
    // ... (omitted for brevity in this phase)
    
    return true;
}

// ============================================================================
// ADVERTISING
// ============================================================================

bool BLEManager::startAdvertising() {
    if (!initialized || !advertising) return false;
    
    advertising->start();
    advertisingActive = true;
    LogManager::info("BLE Advertising started");
    return true;
}

void BLEManager::stopAdvertising() {
    if (advertising) {
        advertising->stop();
    }
    advertisingActive = false;
    LogManager::info("BLE Advertising stopped");
}

// ============================================================================
// GATT OPERATIONS
// ============================================================================

bool BLEManager::notify(const String& uuid, const std::vector<uint8_t>& data) {
    BLECharacteristic* targetChar = nullptr;
    
    if (uuid == WIBLE_STATUS_CHARACTERISTIC) targetChar = statusChar;
    else if (uuid == WIBLE_DATA_CHARACTERISTIC) targetChar = dataChar;
    
    if (targetChar) {
        targetChar->setValue((uint8_t*)data.data(), data.size());
        targetChar->notify();
        return true;
    }
    return false;
}

// ============================================================================
// CALLBACKS
// ============================================================================

void BLEManager::ServerCallbacks::onConnect(BLEServer* server) {
    LogManager::info("BLE Client Connected");
    // In real implementation, we would get the address and update connection info
    if (manager->connectionCallback) {
        BLEConnectionInfo info;
        info.connectedAt = millis();
        manager->connectionCallback(info);
    }
}

void BLEManager::ServerCallbacks::onDisconnect(BLEServer* server) {
    LogManager::info("BLE Client Disconnected");
    if (manager->disconnectionCallback) {
        manager->disconnectionCallback("", 0);
    }
    
    // Restart advertising if auto-reconnect or configured to do so
    if (manager->initialized) {
        server->startAdvertising();
    }
}

void BLEManager::CharacteristicCallbacks::onWrite(BLECharacteristic* characteristic) {
    std::string value = characteristic->getValue();
    std::vector<uint8_t> data(value.begin(), value.end());
    
    if (data.empty()) return;
    
    LogManager::debug("BLE Write to " + characteristicUUID + ", len: " + String((int)data.size()));
    
    if (manager->dataReceivedCallback) {
        manager->dataReceivedCallback(characteristicUUID, data);
    }
}

void BLEManager::CharacteristicCallbacks::onRead(BLECharacteristic* characteristic) {
    // Handle read request if needed
}

void BLEManager::CharacteristicCallbacks::onNotify(BLECharacteristic* characteristic) {
    // Handle notify confirmation if needed
}

// ============================================================================
// PLACEHOLDERS
// ============================================================================

bool BLEManager::isAdvertising() const { return advertisingActive; }
bool BLEManager::isConnected() const { return bleServer->getConnectedCount() > 0; }
uint8_t BLEManager::getConnectionCount() const { return bleServer->getConnectedCount(); }
void BLEManager::processOperationQueue() { /* TODO: Implement queue processing */ }
void BLEManager::onConnection(BLEConnectionCallback callback) { connectionCallback = callback; }
void BLEManager::setScanCallback(BLEScanCallback callback) {
    scanCallback = callback;
}

// ============================================================================
// BEACON MODE
// ============================================================================

void BLEManager::startBeacon(String uuid, uint16_t major, uint16_t minor, int8_t rssiAt1m) {
    if (!initialized || !advertising) return;

    stopAdvertising(); // Stop any existing advertising

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
    
    // iBeacon Flag (0x020106)
    oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
    
    // Construct iBeacon Payload
    // Format: [Len] [Type] [CompanyID(2)] [BeaconType(2)] [ProximityUUID(16)] [Major(2)] [Minor(2)] [TxPower(1)]
    // Total Manufacturer Data Length: 25 bytes
    
    std::string mfgData;
    mfgData += (char)0x4C; mfgData += (char)0x00; // Apple Company ID (0x004C)
    mfgData += (char)0x02; mfgData += (char)0x15; // iBeacon Type (0x02, 0x15)
    
    // UUID (Need to parse string to bytes)
    // For simplicity in this implementation, we'll assume a fixed UUID or parse it manually
    // A real implementation would use a UUID parser. 
    // Let's use a dummy UUID for now if parsing is complex, or try basic parsing.
    // Actually, BLEUUID class can parse it.
    BLEUUID bleUUID(uuid.c_str());
    esp_bt_uuid_t rawUUID = bleUUID.getNative();
    
    // Check if it's 128-bit
    if (rawUUID.len == 16) {
        for(int i=0; i<16; i++) mfgData += (char)rawUUID.uuid.uuid128[i];
    } else {
        // Fallback or error
        LogManager::error("Invalid Beacon UUID length");
        return;
    }

    mfgData += (char)((major >> 8) & 0xFF);
    mfgData += (char)(major & 0xFF);
    
    mfgData += (char)((minor >> 8) & 0xFF);
    mfgData += (char)(minor & 0xFF);
    
    mfgData += (char)rssiAt1m;
    
    oAdvertisementData.setManufacturerData(mfgData);
    
    advertising->setAdvertisementData(oAdvertisementData);
    advertising->setScanResponseData(oScanResponseData);
    
    advertising->start();
    advertisingActive = true;
    LogManager::info("iBeacon started");
}
void BLEManager::onDisconnection(BLEDisconnectionCallback callback) { disconnectionCallback = callback; }
void BLEManager::onDataReceived(BLEDataReceivedCallback callback) { dataReceivedCallback = callback; }

} // namespace WiBLE
