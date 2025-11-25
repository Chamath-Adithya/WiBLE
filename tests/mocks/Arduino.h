#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

// Mock String class
class String : public std::string {
public:
    String() : std::string() {}
    String(const char* s) : std::string(s ? s : "") {}
    String(const std::string& s) : std::string(s) {}
    String(int i) : std::string(std::to_string(i)) {}
    String(const char* s, size_t len) : std::string(s, len) {}
    
    bool isEmpty() const { return empty(); }
    const char* c_str() const { return std::string::c_str(); }
    
    // Operator overloading for concatenation
    String operator+(const String& other) const {
        return String(std::string(*this) + std::string(other));
    }

    String operator+(const char* other) const {
        return String(std::string(*this) + std::string(other));
    }
    
    int indexOf(const char* str, int fromIndex = 0) const {
        size_t pos = std::string::find(str, fromIndex);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }
    
    String substring(int from, int to = -1) const {
        if (to == -1) return String(std::string::substr(from));
        return String(std::string::substr(from, to - from));
    }
};

// Mock time functions
inline uint32_t millis() { return 0; }
inline void delay(uint32_t ms) {}

// Mock Serial
class SerialMock {
public:
    void begin(unsigned long baud) {}
    void println(const String& s) { std::cout << s.c_str() << std::endl; }
    void print(const String& s) { std::cout << s.c_str(); }
};

extern SerialMock Serial;

#endif // ARDUINO_H
