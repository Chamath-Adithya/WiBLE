# SensorDashboard Protocol Documentation

This document explains the communication protocol used by the SensorDashboard example.

## Overview

SensorDashboard demonstrates **bidirectional data transfer** between ESP32 and a mobile app:
- **Device → App**: 4 sensor readings (temperature, humidity, light, motion)
- **App → Device**: 2 LED control signals

## Hardware Connections

```
ESP32          Component
------         ---------
GPIO 2    ──→  LED1 (with 220Ω resistor to GND)
GPIO 4    ──→  LED2 (with 220Ω resistor to GND)
GPIO 34   ──→  Temperature Sensor (analog)
GPIO 35   ──→  Humidity Sensor (analog)
GPIO 36   ──→  Light Sensor (LDR + 10kΩ resistor)
GPIO 39   ──→  Motion Sensor (PIR output)
```

## Protocol Specification

### Commands: App → Device (Control)

| Command ID | Name | Payload | Description |
|------------|------|---------|-------------|
| `0x01` | LED1_CONTROL | 1 byte | Control LED1: `0x00`=OFF, `0x01`=ON |
| `0x02` | LED2_CONTROL | 1 byte | Control LED2: `0x00`=OFF, `0x01`=ON |

**Example Packets**:
```
Turn ON LED1:  [0x01, 0x01]
Turn OFF LED1: [0x01, 0x00]
Turn ON LED2:  [0x02, 0x01]
Turn OFF LED2: [0x02, 0x00]
```

### Commands: Device → App (Telemetry)

| Command ID | Name | Payload | Description |
|------------|------|---------|-------------|
| `0x10` | SENSOR_DATA | 11 bytes | All sensor readings |

**Packet Structure** (12 bytes total):
```
Byte 0:     Command ID (0x10)
Bytes 1-4:  Temperature (float, little-endian) in °C
Bytes 5-8:  Humidity (float, little-endian) in %
Bytes 9-10: Light (uint16_t, little-endian) 0-4095
Byte 11:    Motion (uint8_t) 0=No motion, 1=Motion detected
```

## Mobile App Implementation Guide

### Receiving Sensor Data (JavaScript/TypeScript)

```javascript
// Parse received sensor data
function parseSensorData(dataView) {
    if (dataView.byteLength !== 12 || dataView.getUint8(0) !== 0x10) {
        console.error('Invalid sensor packet');
        return null;
    }
    
    return {
        temperature: dataView.getFloat32(1, true),  // true = little-endian
        humidity: dataView.getFloat32(5, true),
        light: dataView.getUint16(9, true),
        motion: dataView.getUint8(11) === 1
    };
}

// Example usage
bleCharacteristic.addEventListener('characteristicvaluechanged', (event) => {
    const sensors = parseSensorData(event.target.value);
    console.log(`Temp: ${sensors.temperature.toFixed(1)}°C`);
    console.log(`Humidity: ${sensors.humidity.toFixed(1)}%`);
    console.log(`Light: ${sensors.light}`);
    console.log(`Motion: ${sensors.motion ? 'Detected' : 'None'}`);
});
```

### Sending LED Commands (JavaScript/TypeScript)

```javascript
async function setLED1(state) {
    const command = new Uint8Array([0x01, state ? 0x01 : 0x00]);
    await bleCharacteristic.writeValue(command);
}

async function setLED2(state) {
    const command = new Uint8Array([0x02, state ? 0x01 : 0x00]);
    await bleCharacteristic.writeValue(command);
}
```

## Testing Without Hardware

The example works without physical sensors by simulating values. You can test with:
- **LED1, LED2**: Built-in ESP32 GPIO pins (observe with multimeter or connect LEDs)
- **Sensors**: Reads analog pins and converts to simulated realistic values

## Extending the Protocol

To add more sensors or actuators:

1. **Define new command IDs** (avoid conflicts):
   ```cpp
   #define CMD_SERVO_CONTROL  0x03  // New actuator
   #define CMD_PRESSURE_DATA  0x11  // New sensor
   ```

2. **Add handler in `onDataReceived()`**:
   ```cpp
   case CMD_SERVO_CONTROL:
       setServoAngle(data[1]);
       break;
   ```

3. **Modify `sendSensorDataToApp()`** to include new data:
   ```cpp
   packet[12] = pressureValue;  // Add to existing packet
   ```

## Sample Mobile App UI

Here's a simple HTML/JS dashboard you can use:

```html
<!DOCTYPE html>
<html>
<head>
    <title>WiBLE Dashboard</title>
</head>
<body>
    <h1>IoT Sensor Dashboard</h1>
    
    <div>
        <h2>Sensor Readings</h2>
        <p>🌡️ Temperature: <span id="temp">--</span>°C</p>
        <p>💧 Humidity: <span id="humidity">--</span>%</p>
        <p>💡 Light: <span id="light">--</span></p>
        <p>🚶 Motion: <span id="motion">--</span></p>
    </div>
    
    <div>
        <h2>LED Control</h2>
        <button onclick="toggleLED1()">LED1: <span id="led1">OFF</span></button>
        <button onclick="toggleLED2()">LED2: <span id="led2">OFF</span></button>
    </div>
    
    <script>
        let led1State = false;
        let led2State = false;
        let characteristic = null;
        
        async function connect() {
            const device = await navigator.bluetooth.requestDevice({
                filters: [{ name: 'WiBLE_Dashboard' }],
                optionalServices: ['6e400001-b5a3-f393-e0a9-e50e24dcca9e']
            });
            
            const server = await device.gatt.connect();
            const service = await server.getPrimaryService('6e400001-b5a3-f393-e0a9-e50e24dcca9e');
            characteristic = await service.getCharacteristic('6e400005-b5a3-f393-e0a9-e50e24dcca9e');
            
            // Listen for sensor data
            characteristic.addEventListener('characteristicvaluechanged', (e) => {
                const data = parseSensorData(e.target.value);
                document.getElementById('temp').textContent = data.temperature.toFixed(1);
                document.getElementById('humidity').textContent = data.humidity.toFixed(1);
                document.getElementById('light').textContent = data.light;
                document.getElementById('motion').textContent = data.motion ? 'Detected' : 'None';
            });
            
            await characteristic.startNotifications();
        }
        
        async function toggleLED1() {
            led1State = !led1State;
            await characteristic.writeValue(new Uint8Array([0x01, led1State ? 1 : 0]));
            document.getElementById('led1').textContent = led1State ? 'ON' : 'OFF';
        }
        
        async function toggleLED2() {
            led2State = !led2State;
            await characteristic.writeValue(new Uint8Array([0x02, led2State ? 1 : 0]));
            document.getElementById('led2').textContent = led2State ? 'ON' : 'OFF';
        }
        
        // Auto-connect on load
        connect().catch(console.error);
    </script>
</body>
</html>
```

---

**Happy Building!** 🚀
