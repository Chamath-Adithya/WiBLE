#ifndef PUBSUBCLIENT_H
#define PUBSUBCLIENT_H

#include <Arduino.h>
#include <functional>

// Mock Client class (base for WiFiClient)
class Client {
public:
    virtual int connect(const char*, uint16_t) { return 1; }
    virtual size_t write(const uint8_t*, size_t) { return 0; }
    virtual int read() { return -1; }
    virtual void stop() {}
    virtual uint8_t connected() { return 1; }
};

class WiFiClient : public Client {};

class PubSubClient {
public:
    PubSubClient(Client& client) {}
    void setServer(const char* host, uint16_t port) {}
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)>) {}
    bool connect(const char* id) { return true; }
    bool connect(const char* id, const char* user, const char* pass) { return true; }
    void loop() {}
    void publish(const char* topic, const char* payload) {}
    void subscribe(const char* topic) {}
    bool connected() { return false; } // Default to false to trigger reconnect logic in example
};

#endif
