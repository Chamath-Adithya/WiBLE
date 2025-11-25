#ifndef BLE_DEVICE_MOCK_H
#define BLE_DEVICE_MOCK_H

#include "Arduino.h"
#include <vector>
#include <string>
#include <functional>

// Mock ESP32 UUID types
typedef struct {
    uint16_t len;
    union {
        uint16_t uuid16;
        uint32_t uuid32;
        uint8_t uuid128[16];
    } uuid;
} esp_bt_uuid_t;

class BLEUUID {
public:
    BLEUUID(const char*) {}
    esp_bt_uuid_t getNative() {
        esp_bt_uuid_t u;
        u.len = 16; // Mock as 128-bit
        return u;
    }
};

class BLEAddress {
public:
    BLEAddress() {}
};

class BLEServerCallbacks;
class BLECharacteristicCallbacks;

class BLEDescriptor {
public:
    BLEDescriptor() {}
};

class BLE2902 : public BLEDescriptor {
public:
    BLE2902() {}
};

class BLECharacteristic {
public:
    static const uint32_t PROPERTY_READ   = 1<<0;
    static const uint32_t PROPERTY_WRITE  = 1<<1;
    static const uint32_t PROPERTY_NOTIFY = 1<<2;

    void setCallbacks(BLECharacteristicCallbacks* callbacks) {}
    void addDescriptor(BLEDescriptor* descriptor) {}
    void setValue(uint8_t* data, size_t size) {}
    void notify() {}
    std::string getValue() { return ""; }
};

class BLEService {
public:
    BLECharacteristic* createCharacteristic(const char* uuid, uint32_t properties) {
        return new BLECharacteristic();
    }
    void start() {}
};

class BLEAdvertisementData {
public:
    void setFlags(uint8_t) {}
    void setManufacturerData(std::string) {}
    void setServiceUUID(const char*) {}
};

class BLEAdvertising {
public:
    void addServiceUUID(const char* uuid) {}
    void start() {}
    void stop() {}
    void setScanResponse(bool) {}
    void setMinPreferred(uint8_t) {}
    void setAdvertisementData(BLEAdvertisementData&) {}
    void setScanResponseData(BLEAdvertisementData&) {}
};

class BLEScanResults {
public:
    // Minimal mock for BLEScanResults
};

class BLEScan {
public:
    void setActiveScan(bool) {}
    void setInterval(uint16_t) {}
    void setWindow(uint16_t) {}
    BLEScanResults start(uint32_t, bool) { return BLEScanResults(); }
    void stop() {}
    void clearResults() {}
};

class BLEServer {
public:
    void setCallbacks(BLEServerCallbacks* callbacks) {}
    BLEService* createService(const char* uuid) { return new BLEService(); }
    void startAdvertising() {}
    int getConnectedCount() { return 0; }
};

class BLEDevice {
public:
    static void init(std::string) {}
    static BLEServer* createServer() { return new BLEServer(); }
    static BLEAdvertising* getAdvertising() { return new BLEAdvertising(); }
    static BLEScan* getScan() { return new BLEScan(); }
};

class BLEServerCallbacks {
public:
    virtual ~BLEServerCallbacks() {}
    virtual void onConnect(BLEServer* server) {}
    virtual void onDisconnect(BLEServer* server) {}
    virtual void onMtuChanged(BLEServer* server, uint16_t mtu) {}
};

class BLECharacteristicCallbacks {
public:
    virtual ~BLECharacteristicCallbacks() {}
    virtual void onWrite(BLECharacteristic* characteristic) {}
    virtual void onRead(BLECharacteristic* characteristic) {}
    virtual void onNotify(BLECharacteristic* characteristic) {}
};

#endif
