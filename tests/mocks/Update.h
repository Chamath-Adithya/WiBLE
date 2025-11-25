#ifndef UPDATE_H
#define UPDATE_H

#include <Arduino.h>

class UpdateClass {
public:
    bool begin(size_t size, int command = 0) { return true; }
    size_t write(uint8_t* data, size_t len) { return len; }
    bool end(bool evenIfRemaining = true) { return true; }
    bool hasError() { return false; }
    int getError() { return 0; }
};

extern UpdateClass Update;

#endif
