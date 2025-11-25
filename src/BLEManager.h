/**
 * BLEManager.h - BLE Management for WiBLE
 * 
 * Handles BLE advertising, connections, GATT services,
 * MTU negotiation, and connection parameters.
 */

#ifndef WIBLE_BLE_MANAGER_H
#define WIBLE_BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <functional>
#include <map>
#include <queue>
#include <memory>
#include <freertos/semphr.h>

namespace WiBLE {

// ============================================================================
// BLE CONSTANTS & UUIDs
// ============================================================================

// Custom WiBLE Service UUID
#define WIBLE_SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"

// Characteristics
#define WIBLE_CRED_CHARACTERISTIC    "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // Write
#define WIBLE_STATUS_CHARACTERISTIC  "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  // Notify
#define WIBLE_CONTROL_CHARACTERISTIC "6e400004-b5a3-f393-e0a9-e50e24dcca9e"  // Write
#define WIBLE_DATA_CHARACTERISTIC    "6e400005-b5a3-f393-e0a9-e50e24dcca9e"  // Read/Write/Notify

// Device Information Service (standard)
#define DEVICE_INFO_SERVICE_UUID     "180a"
#define DEVICE_NAME_CHAR_UUID        "2a00"
#define MANUFACTURER_CHAR_UUID       "2a29"
#define FIRMWARE_VERSION_CHAR_UUID   "2a26"

// ============================================================================
// BLE CONFIGURATION
// ============================================================================

struct BLEConfig {
    String deviceName = "WiBLE_Device";
    String manufacturerName = "WiBLE";
    String firmwareVersion = "2.0.0";
    
    // Connection parameters
    uint16_t mtuSize = 512;
    uint16_t connectionInterval = 24;  // 30ms (units of 1.25ms)
    uint16_t slaveLatency = 0;
    uint16_t supervisionTimeout = 400; // 4s (units of 10ms)
    
    // Advertising parameters
    uint32_t advertisingIntervalMs = 100;
    bool advertisingEnabled = true;
    bool scanResponseEnabled = true;
    
    // Security
    bool enableBonding = true;
    bool enableSecureConnection = true;
    uint8_t minKeySize = 7;
    uint8_t maxKeySize = 16;
    
    // Power management
    int8_t txPowerLevel = 3;  // dBm: -12, -9, -6, -3, 0, 3, 6, 9
    bool enablePowerSaving = false;
    
    // Connection management
    uint8_t maxConnections = 1;
    uint32_t connectionTimeoutMs = 30000;
    bool autoReconnect = false;
};

// ============================================================================
// BLE CONNECTION INFO
// ============================================================================

struct BLEConnectionInfo {
    String clientAddress;
    uint16_t connectionId;
    uint16_t mtu;
    int8_t rssi;
    uint32_t connectedAt;
    uint32_t lastActivityAt;
    bool isAuthenticated;
    bool isNotifyEnabled;
    
    BLEConnectionInfo() : connectionId(0), mtu(23), rssi(0), 
                         connectedAt(0), lastActivityAt(0),
                         isAuthenticated(false), isNotifyEnabled(false) {}
    
    uint32_t getConnectionDuration() const {
        return millis() - connectedAt;
    }
    
    uint32_t getIdleTime() const {
        return millis() - lastActivityAt;
    }
};

// ============================================================================
// GATT OPERATION
// ============================================================================

enum class GATTOperationType {
    READ,
    WRITE,
    WRITE_NO_RESPONSE,
    NOTIFY,
    INDICATE
};

struct GATTOperation {
    GATTOperationType type;
    String characteristicUUID;
    std::vector<uint8_t> data;
    std::function<void(bool success, const std::vector<uint8_t>& response)> callback;
    uint32_t timestamp;
    uint8_t retryCount;
    uint8_t maxRetries;
    
    GATTOperation() : timestamp(millis()), retryCount(0), maxRetries(3) {}
};

// ============================================================================
// BLE CALLBACKS
// ============================================================================

using BLEConnectionCallback = std::function<void(const BLEConnectionInfo& info)>;
using BLEDisconnectionCallback = std::function<void(String address, uint8_t reason)>;
using BLEDataReceivedCallback = std::function<void(const String& characteristicUUID, 
                                                   const std::vector<uint8_t>& data)>;
using MTUChangeCallback = std::function<void(uint16_t mtu)>;
using RSSIUpdateCallback = std::function<void(int8_t rssi)>;

// ============================================================================
// BLE MANAGER CLASS
// ============================================================================

class BLEManager {
public:
    BLEManager();
    ~BLEManager();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * Initialize BLE subsystem
     */
    bool initialize(const BLEConfig& config = BLEConfig());
    
    /**
     * Cleanup BLE resources
     */
    void cleanup();
    
    /**
     * Check if BLE is initialized
     */
    bool isInitialized() const { return initialized; }
    
    // ========================================================================
    // ADVERTISING
    // ========================================================================
    
    /**
     * Start BLE advertising
     */
    bool startAdvertising();
    
    /**
     * Stop BLE advertising
     */
    void stopAdvertising();
    
    /**
     * Check if advertising
     */
    bool isAdvertising() const;
    
    /**
     * Update advertising data
     */
    void setAdvertisingData(const std::vector<uint8_t>& manufacturerData);

    /**
     * Update advertising data with raw bytes
     */
    void updateAdvertisingData(uint8_t* data, size_t length);

    /**
     * Set manufacturer data helper
     */
    void setManufacturerData(uint16_t companyId, uint8_t* data, size_t length);
    
    /**
     * Set scan response data
     */
    void setScanResponseData(const std::vector<uint8_t>& data);
    
    // ========================================================================
    // CONNECTION MANAGEMENT
    // ========================================================================
    
    /**
     * Check if any client is connected
     */
    bool isConnected() const;
    
    /**
     * Get number of connected clients
     */
    uint8_t getConnectionCount() const;
    
    /**
     * Get connection info
     */
    BLEConnectionInfo getConnectionInfo(const String& address) const;
    
    /**
     * Get all connected clients
     */
    std::vector<String> getConnectedClients() const;
    
    /**
     * Disconnect specific client
     */
    void disconnect(const String& address);
    
    /**
     * Disconnect all clients
     */
    void disconnectAll();
    
    /**
     * Update connection parameters
     */
    bool updateConnectionParameters(uint16_t minInterval, uint16_t maxInterval,
                                   uint16_t latency, uint16_t timeout);
    
    // ========================================================================
    // MTU NEGOTIATION
    // ========================================================================
    
    /**
     * Request MTU size (called automatically on connection)
     */
    bool requestMTU(uint16_t size);
    
    /**
     * Get current MTU
     */
    uint16_t getMTU() const;
    
    /**
     * Get maximum payload size (MTU - 3 bytes overhead)
     */
    uint16_t getMaxPayloadSize() const;
    
    // ========================================================================
    // GATT OPERATIONS
    // ========================================================================
    
    /**
     * Write data to characteristic
     */
    bool writeCharacteristic(const String& uuid, const std::vector<uint8_t>& data);
    
    /**
     * Write data (convenience for strings)
     */
    bool writeCharacteristic(const String& uuid, const String& data);
    
    /**
     * Read characteristic
     */
    std::vector<uint8_t> readCharacteristic(const String& uuid);
    
    /**
     * Send notification to client
     */
    bool notify(const String& uuid, const std::vector<uint8_t>& data);
    
    /**
     * Send notification (string)
     */
    bool notify(const String& uuid, const String& data);
    
    /**
     * Send indication (requires acknowledgment)
     */
    bool indicate(const String& uuid, const std::vector<uint8_t>& data);
    
    /**
     * Enable/disable notifications for client
     */
    bool setNotificationsEnabled(const String& uuid, bool enabled);
    
    // ========================================================================
    // OPERATION QUEUE
    // ========================================================================
    
    /**
     * Enqueue GATT operation (for serialization)
     */
    void enqueueOperation(const GATTOperation& operation);
    
    /**
     * Process operation queue (call in loop)
     */
    void processOperationQueue();
    
    /**
     * Clear operation queue
     */
    void clearOperationQueue();
    
    /**
     * Get queue size
     */
    size_t getQueueSize() const;
    
    // ========================================================================
    // CHUNKED DATA TRANSFER
    // ========================================================================
    
    /**
     * Send large data in chunks (handles MTU automatically)
     */
    bool sendLargeData(const std::vector<uint8_t>& data, 
                      std::function<void(uint8_t progress)> progressCallback = nullptr);
    
    /**
     * Receive large data (reassembles chunks)
     */
    void handleIncomingChunk(const std::vector<uint8_t>& chunk);
    
    // ========================================================================
    // RSSI & SIGNAL STRENGTH
    // ========================================================================
    
    /**
     * Read RSSI for connected client
     */
    int8_t readRSSI(const String& address);
    
    /**
     * Start RSSI monitoring (periodic updates)
     */
    void startRSSIMonitoring(uint32_t intervalMs = 1000);
    
    /**
     * Stop RSSI monitoring
     */
    void stopRSSIMonitoring();
    
    // ========================================================================
    // CALLBACKS
    // ========================================================================
    
    void onConnection(BLEConnectionCallback callback);
    void onDisconnection(BLEDisconnectionCallback callback);
    void onDataReceived(BLEDataReceivedCallback callback);
    void onMTUChange(MTUChangeCallback callback);
    void onRSSIUpdate(RSSIUpdateCallback callback);
    
    // ========================================================================
    // UTILITY
    // ========================================================================
    
    /**
     * Get BLE MAC address
     */
    String getMACAddress() const;
    
    /**
     * Get device name
     */
    String getDeviceName() const;
    
    /**
     * Set device name (updates advertising)
     */
    void setDeviceName(const String& name);
    
    /**
     * Check BLE availability
     */
    static bool isBluetoothAvailable();
    
    /**
     * Get BLE stack version
     */
    String getStackVersion() const;
    
    // ========================================================================
    // DEBUGGING
    // ========================================================================
    
    void dumpConnections() const;
    void dumpServices() const;
    void dumpStatistics() const;

private:
    // ESP32 BLE Objects
    BLEServer* bleServer;
    BLEService* provisioningService;
    BLEService* deviceInfoService;
    BLEAdvertising* advertising;
    
    // Characteristics
    BLECharacteristic* credentialsChar;
    BLECharacteristic* statusChar;
    BLECharacteristic* controlChar;
    BLECharacteristic* dataChar;
    
    // Configuration
    BLEConfig config;
    
    // Connection tracking
    std::map<String, BLEConnectionInfo> connections;
    String primaryClientAddress;
    
    // Operation queue
    std::queue<GATTOperation> operationQueue;
    bool processingOperation;
    SemaphoreHandle_t queueMutex;
    
    // Chunked transfer state
    struct ChunkedTransfer {
        std::vector<uint8_t> buffer;
        size_t expectedSize;
        size_t receivedSize;
        uint32_t startTime;
        bool inProgress;
    } chunkedTransfer;
    
    // Callbacks
    BLEConnectionCallback connectionCallback;
    BLEDisconnectionCallback disconnectionCallback;
    BLEDataReceivedCallback dataReceivedCallback;
    MTUChangeCallback mtuChangeCallback;
    RSSIUpdateCallback rssiUpdateCallback;
    
    // RSSI monitoring
    bool rssiMonitoringEnabled;
    uint32_t rssiMonitorInterval;
    uint32_t lastRSSIUpdate;
    
    // State
    bool initialized;
    bool advertisingActive;
    uint32_t initTime;
    
    // Statistics
    struct {
        uint32_t totalConnections;
        uint32_t totalDisconnections;
        uint32_t totalBytesReceived;
        uint32_t totalBytesSent;
        uint32_t failedOperations;
    } statistics;
    
    // Internal methods
    bool initializeServices();
    bool initializeCharacteristics();
    void setupCallbacks();
    void handleConnection(const String& address);
    void handleDisconnection(const String& address, uint8_t reason);
    void handleCharacteristicWrite(const String& uuid, const std::vector<uint8_t>& data);
    bool executeOperation(const GATTOperation& operation);
    std::vector<std::vector<uint8_t>> chunkData(const std::vector<uint8_t>& data, size_t chunkSize);
    void updateStatistics(uint32_t bytesReceived, uint32_t bytesSent);
    
    // Server callbacks (static wrapper to member functions)
    class ServerCallbacks;
    class CharacteristicCallbacks;
    
    friend class ServerCallbacks;
    friend class CharacteristicCallbacks;
};

// ============================================================================
// BLE SERVER CALLBACKS
// ============================================================================

class BLEManager::ServerCallbacks : public BLEServerCallbacks {
public:
    ServerCallbacks(BLEManager* manager) : manager(manager) {}
    
    void onConnect(BLEServer* server) override;
    void onDisconnect(BLEServer* server) override;
    void onMtuChanged(BLEServer* server, uint16_t mtu) override;

private:
    BLEManager* manager;
};

// ============================================================================
// CHARACTERISTIC CALLBACKS
// ============================================================================

class BLEManager::CharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    CharacteristicCallbacks(BLEManager* manager, String uuid) 
        : manager(manager), characteristicUUID(uuid) {}
    
    void onWrite(BLECharacteristic* characteristic) override;
    void onRead(BLECharacteristic* characteristic) override;
    void onNotify(BLECharacteristic* characteristic) override;

private:
    BLEManager* manager;
    String characteristicUUID;
};

// ============================================================================
// BLE UTILITIES
// ============================================================================

class BLEUtils {
public:
    /**
     * Convert BLE address to string
     */
    static String addressToString(const uint8_t* address);
    
    /**
     * Parse advertising data
     */
    static std::map<uint8_t, std::vector<uint8_t>> parseAdvertisingData(
        const std::vector<uint8_t>& data);
    
    /**
     * Build advertising data
     */
    static std::vector<uint8_t> buildAdvertisingData(
        const std::map<uint8_t, std::vector<uint8_t>>& elements);
    
    /**
     * Calculate optimal MTU
     */
    static uint16_t calculateOptimalMTU(size_t dataSize);
    
    /**
     * Validate UUID format
     */
    static bool isValidUUID(const String& uuid);
    
    /**
     * Get disconnect reason string
     */
    static String getDisconnectReasonString(uint8_t reason);
};

} // namespace WiBLE

#endif // WIBLE_BLE_MANAGER_H
