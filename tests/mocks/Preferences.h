#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "Arduino.h"

class Preferences {
public:
    bool begin(const char* name, bool readOnly = false) { return true; }
    void end() {}
    size_t putBytes(const char* key, const void* value, size_t len) { return len; }
    size_t getBytes(const char* key, void* value, size_t len) { return len; }
    void clear() {}
};

#endif
