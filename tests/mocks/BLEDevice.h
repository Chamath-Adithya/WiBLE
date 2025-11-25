#ifndef BLE_DEVICE_MOCK_H
#define BLE_DEVICE_MOCK_H

#include "Arduino.h"

class BLEServer {};
class BLEService {};
class BLECharacteristic {};
class BLEAdvertising {};

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
