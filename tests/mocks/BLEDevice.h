#ifndef BLE_DEVICE_MOCK_H
#define BLE_DEVICE_MOCK_H

#include "Arduino.h"
#include <vector>
#include <string>

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

class BLEAdvertising {
public:
    void addServiceUUID(const char* uuid) {}
    void setScanResponse(bool) {}
    void setMinPreferred(uint32_t) {}
    void start() {}
    void stop() {}
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
    static void init(const char* name) {}
    static BLEServer* createServer() { return new BLEServer(); }
    static BLEAdvertising* getAdvertising() { return new BLEAdvertising(); }
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
