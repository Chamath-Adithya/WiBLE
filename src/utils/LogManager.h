/**
 * LogManager.h - Simple logging utility for WiBLE
 */

#ifndef WIBLE_LOG_MANAGER_H
#define WIBLE_LOG_MANAGER_H

#include <Arduino.h>
#include "../WiBLE.h"

namespace WiBLE {

class LogManager {
public:
    static void log(LogLevel level, const String& message) {
        // Simple implementation for now, can be expanded later
        // In a real implementation, we would check config.logLevel
        // For now, we assume INFO level and Serial enabled
        
        String prefix = "";
        switch (level) {
            case LogLevel::VERBOSE: prefix = "[VERB] "; break;
            case LogLevel::DEBUG:   prefix = "[DEBG] "; break;
            case LogLevel::INFO:    prefix = "[INFO] "; break;
            case LogLevel::WARN:    prefix = "[WARN] "; break;
            case LogLevel::ERROR:   prefix = "[ERR ] "; break;
            default: break;
        }
        
        Serial.println(prefix + message);
    }
    
    static void info(const String& message) {
        log(LogLevel::INFO, message);
    }
    
    static void error(const String& message) {
        log(LogLevel::ERROR, message);
    }
    
    static void debug(const String& message) {
        log(LogLevel::DEBUG, message);
    }

    static void warn(const String& message) {
        log(LogLevel::WARN, message);
    }
};

} // namespace WiBLE

#endif // WIBLE_LOG_MANAGER_H
