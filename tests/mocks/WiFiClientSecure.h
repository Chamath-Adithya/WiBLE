#ifndef WIFICLIENTSECURE_H
#define WIFICLIENTSECURE_H

#include "Arduino.h"
#include "PubSubClient.h" // Reuse Client base class

class WiFiClientSecure : public Client {
public:
    void setCACert(const char* rootCA) {}
    void setCertificate(const char* clientCert) {}
    void setPrivateKey(const char* privateKey) {}
};

#endif
