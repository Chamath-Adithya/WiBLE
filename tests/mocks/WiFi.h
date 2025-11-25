#ifndef WIFI_MOCK_H
#define WIFI_MOCK_H

#include "Arduino.h"

typedef int wifi_auth_mode_t;
typedef int wifi_ps_type_t;
typedef int wifi_power_t;
typedef int WiFiEvent_t;

#define WIFI_PS_MIN_MODEM 1

class WiFiClass {};
extern WiFiClass WiFi;

#endif
