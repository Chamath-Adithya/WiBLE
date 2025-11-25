#ifndef WIFI_MOCK_H
#define WIFI_MOCK_H

#include "Arduino.h"
#include <vector>

#define WIFI_STA 1
#define WL_CONNECTED 3
#define WIFI_AUTH_OPEN 0
#define WIFI_AUTH_WEP 1
#define WIFI_AUTH_WPA_PSK 2
#define WIFI_AUTH_WPA2_PSK 3
#define WIFI_AUTH_WPA_WPA2_PSK 4
#define WIFI_AUTH_WPA2_ENTERPRISE 5

#define WIFI_PS_MIN_MODEM 1

// Mock types
typedef int wifi_power_t;
typedef int wifi_ps_type_t;
typedef int wifi_auth_mode_t;
typedef int WiFiEvent_t;

class IPAddress {
public:
    String toString() const { return "192.168.1.100"; }
};

class WiFiClass {
public:
    void mode(int m) {}
    void setAutoReconnect(bool b) {}
    void begin(const char* ssid, const char* pass) {}
    int status() { return WL_CONNECTED; }
    void disconnect(bool wifioff = false) {}
    
    void scanNetworks(bool async) {}
    int16_t scanComplete() { return 1; }
    void scanDelete() {}
    
    String SSID(int i) { return "TestNetwork"; }
    String SSID() { return "TestNetwork"; }
    int32_t RSSI(int i) { return -60; }
    int32_t RSSI() { return -60; }
    int32_t channel(int i) { return 6; }
    int encryptionType(int i) { return WIFI_AUTH_WPA2_PSK; }
    
    IPAddress localIP() { return IPAddress(); }
    IPAddress gatewayIP() { return IPAddress(); }
    IPAddress subnetMask() { return IPAddress(); }
    String macAddress() { return "00:11:22:33:44:55"; }
};

extern WiFiClass WiFi;

#endif
