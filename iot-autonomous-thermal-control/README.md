# Autonomous Thermal Control System

![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C++-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

IoT-enabled autonomous environmental control system with real-time web dashboard. Built to solve a real problem (overheating AV equipment) while demonstrating embedded systems, wireless communication, and full-stack IoT development principles.

## Project Overview

This system autonomously monitors ambient temperature via DHT11 sensor and controls a cooling fan through relay actuation—all while providing remote access through a WiFi-enabled web dashboard. The ESP32 makes real-time control decisions at the edge while maintaining cloud connectivity for data monitoring and manual override.

**Real-World Context:** My AV receiver was overheating in its TV stand enclosure. Instead of just adding a fan, I built an intelligent system that demonstrates autonomous control, wireless telemetry, and human-in-the-loop override capabilities—principles directly applicable to defense systems, unmanned vehicles, and distributed sensor networks.

## Key Features

### Autonomous Operation
- **Threshold-Based Control** - Automatic fan activation at configurable temperature setpoint
- **Hysteresis Logic** - 1°C dead-band prevents rapid on/off cycling and extends hardware life
- **Edge Computing** - Real-time decisions made locally without cloud dependency
- **Manual Override** - Remote control capability via web interface for testing or emergency response
- **Graceful Degradation** - System maintains last known state if sensor fails

### IoT Connectivity & Monitoring
- **WiFi Web Dashboard** - Full-featured control interface accessible from any device on network
- **Live Data Streaming** - 2-second update rate with Chart.js visualization
- **Historical Graphs** - Rolling 2-minute temperature trend display
- **Remote Control** - Turn fan on/off or toggle automatic mode from anywhere
- **Dynamic Settings** - Adjust temperature threshold without re-programming
- **System Telemetry** - Uptime tracking, connection status, operating mode indicators

### Hardware Integration
- **ESP32 Dual-Core** - Concurrent WiFi communication and real-time control
- **DHT11 Sensor** - Temperature and humidity monitoring with ±2°C accuracy
- **Relay Module** - Isolated 10A switching for AC/DC loads
- **USB Desk Fan** - 5V DC cooling actuator (easily replaceable with larger fans)

## Hardware Architecture

### Components & Specifications

| Component | Function | Interface | Cost |
|-----------|----------|-----------|------|
| ESP32 DevKit (30-pin) | Main controller + WiFi | - | ~$10 |
| DHT11 Temperature Sensor | Environmental monitoring | 1-wire digital (GPIO 25) | ~$3 |
| 5V Relay Module | Load switching | GPIO 26 (active HIGH) | ~$3 |
| USB Desk Fan (5V DC) | Cooling actuator | Relay-switched power | ~$5 |
| Jumper Wires | Interconnection | - | ~$5 |
| Breadboard | Prototyping | - | ~$5 |

**Total Project Cost:** ~$31

### Pin Configuration

```
DHT11 Sensor:
  VCC  → ESP32 3.3V (or 5V if needed for power)
  DATA → GPIO 25
  GND  → ESP32 GND

Relay Module:
  VCC → ESP32 VIN (5V)
  IN  → GPIO 26
  GND → ESP32 GND

Fan Power (through relay):
  USB Fan (+) → Relay COM terminal
  Relay NO → ESP32 VIN or external 5V supply
  USB Fan (-) → ESP32 GND
```

### System Architecture

```
┌───────────────────────────────────────────────────────┐
│         IoT Autonomous Control System                 │
│                                                       │
│  ┌──────────┐      ┌──────────────┐      ┌──────┐     │
│  │  DHT11   │───▶ │    ESP32     │────▶ │Relay │     │
│  │  Sensor  │      │  Dual-Core   │      │Module│     │
│  └──────────┘      │  + WiFi      │      └──┬───┘     │
│                    └───────┬──────┘         │         │
│                            │                ▼         │
│                            │             ┌──────┐     │
│                            │             │ Fan  │     │
│                            ▼             │ 5V DC│     │
│                       ┌──────────┐       └──────┘     │
│                       │   WiFi   │                    │
│                       │Web Server│                    │
│                       └────┬─────┘                    │
│                            │                          │
│                            ▼                          │
│                   ┌────────────────┐                  │
│                   │ Web Dashboard  │                  │
│                   │ (Any Browser)  │                  │
│                   └────────────────┘                  │
└───────────────────────────────────────────────────────┘
```

## Control Algorithm

### Hysteresis-Based Threshold Control

```cpp
// Autonomous control loop (runs every 2 seconds)
if (currentTemp >= TEMP_THRESHOLD && !fanOn) {
    activate_fan();  // Turn on at threshold
}
else if (currentTemp < (TEMP_THRESHOLD - HYSTERESIS) && fanOn) {
    deactivate_fan();  // Turn off 1°C below threshold
}
```

**Parameters:**
- **Threshold**: 22.0°C (configurable via web interface)
- **Hysteresis**: 1.0°C (prevents oscillation)
- **Update Rate**: 2 seconds (sensor polling + display refresh)
- **Data Retention**: 60 readings (2 minutes of history)

### Operating Modes

**Automatic Mode** (Default):
- System makes all control decisions based on temperature
- Fan activates at threshold, deactivates at threshold - hysteresis
- Ideal for "set it and forget it" operation

**Manual Mode**:
- User control via web interface overrides autonomous logic
- ON/OFF buttons for direct fan control
- Useful for testing, maintenance, or forcing cooling
- Remains in manual until "Auto Mode" button clicked

## Web Dashboard

### Real-Time Monitoring
- **Live Temperature Display** - Large, easy-to-read current temperature
- **Humidity Tracking** - Ambient moisture percentage
- **System Uptime** - Hours:Minutes:Seconds since boot
- **Trend Analysis** - Visual identification of heating/cooling patterns

### Interactive Controls
- **Manual Override** - Force fan ON/OFF for testing
- **Auto Mode Toggle** - Return to autonomous operation
- **Threshold Adjustment** - Slider control (18-35°C range)

## Getting Started

### Prerequisites

**Software:**
- Arduino IDE 1.8.13+ or PlatformIO
- ESP32 board support package
- Required libraries:
  - `DHT sensor library` by Adafruit
  - WiFi (built-in to ESP32 core)
  - WebServer (built-in to ESP32 core)

**Hardware:**
- ESP32 development board with WiFi
- WiFi network (Most ESP32's don't support 5GHz)
- USB cable for programming
- 5V power supply (USB adapter or computer)

### Installation

#### 1. Install ESP32 Board Support

**Arduino IDE:**
```
File → Preferences → Additional Board Manager URLs
Add: https://dl.espressif.com/dl/package_esp32_index.json
Tools → Board → Boards Manager → Search "ESP32" → Install
```

#### 2. Install Required Libraries

**Arduino IDE:**
```
Tools → Manage Libraries
Search and install:
  - "DHT sensor library" by Adafruit
  - "Adafruit Unified Sensor" (dependency)
```

#### 3. Clone Repository

```bash
git clone https://github.com/gavinomtz/embedded-systems-projects/tree/main/iot-autonomous-thermal-control
cd iot-autonomous-thermal-control
```

#### 4. Configure WiFi Credentials

Update:

```cpp
const char* ssid = "notleakingmyIP";      // Your network name
const char* password = "notleakingmyPassword";  // Your password
```

**Note:** Make sure you're connecting to the right network. Most ESP32's don't support 5GHz WiFi.

#### 5. Wire Hardware

Follow the pin configuration diagram above. Key connections:
- DHT11 DATA → GPIO 25
- Relay IN → GPIO 26

#### 6. Upload Code

```
1. Connect ESP32 via USB
2. Select Board: "ESP32 Dev Module"
3. Select Port: (your COM/tty port)
4. Click Upload
5. Open Serial Monitor (115200 baud)
6. Wait for "WiFi Connected!" message
7. Note the IP address displayed
```

#### 7. Access Dashboard

```
1. Open web browser (Chrome/Firefox/Safari)
2. Navigate to: http://192.168.1.XXX (ESP32's IP from step 6)
3. Dashboard loads automatically
4. Test manual controls to verify connectivity
```

## Future Enhancements

Custom PCB design to replace the breadboard with a professional 2-layer board integrating the ESP32, power regulation, sensor interface, and relay driver—reducing size by 70% while improving reliability. A 3D-printed enclosure with proper ventilation, mounting points, and cable management. Proportional PWM fan speed control for quieter operation and energy efficiency, PID control algorithms for precise temperature regulation, battery backup for uninterrupted operation during power loss, and capacitive touch interface for local configuration without requiring network access. For commercial deployment, implementing MQTT protocol enables seamless Home Assistant integration, OTA firmware updates allow wireless patches and feature additions, TLS/HTTPS encryption secures the web dashboard, and WPA2-Enterprise authentication provides certificate-based network security for enterprise environments.

## Troubleshooting

### WiFi Connection Issues

**Problem: ESP32 won't connect to WiFi**

**Diagnostics:**
```cpp
// Serial Monitor shows:
Connecting to WiFi..........
WiFi Failed!
```

**Solutions:**
1. Verify SSID and password are correct (case-sensitive)
2. Change network to 2.4GHz (Most ESP32's don't support 5GHz)
3. Check router allows new device connections (not at DHCP limit)
4. Move ESP32 closer to router (weak signal)
5. Try different WiFi channel (interference)


**Problem: Connected but can't access dashboard**

**Diagnostics:**
- ESP32 has IP address
- Dashboard won't load in browser

**Solutions:**
1. Verify device on same WiFi network as ESP32
2. Disable VPN if enabled
3. Try different browser (Chrome recommended)
4. Check firewall not blocking port 80
5. Try pinging ESP32: `ping 192.168.1.XXX`


**Problem: Temperature readings fluctuate wildly**

**Solutions:**
1. Move sensor away from fan airflow
2. Increase polling interval (reduce noise)
3. Implement moving average filter
4. Check for electromagnetic interference
5. Ensure sensor not in direct sunlight

---

### Fan Control Issues

**Problem: Relay clicks but fan doesn't run**

**Diagnostics:**
- Relay audibly clicks
- Fan doesn't spin

**Solutions:**
1. Verify fan requires 5V (not 12V)
2. Check relay NO (Normally Open) terminal used
3. Test fan directly with 5V to confirm functional
4. Ensure adequate power supply current (2A+ recommended)
5. Check USB cable quality (voltage drop)
6. Verify fan polarity correct


**Problem: Fan runs constantly regardless of temperature**

**Diagnostics:**
- Fan always ON even when cold
- Manual controls don't work

**Solutions:**
1. Check relay wiring - may be on NC (Normally Closed) instead of NO
2. Verify GPIO 26 pin number correct
3. Check if manual override stuck enabled
4. Look for code upload errors
5. Try power cycling ESP32

---

### Dashboard Issues

**Problem: Controls work but data shows "--" or old values**

**Diagnostics:**
- Buttons respond
- Temperature/humidity not updating

**Solutions:**
1. Open browser console (F12) - check for JavaScript errors
2. Verify `/data` endpoint returns valid JSON:
   - Visit `http://ESP32_IP/data` directly
   - Should see: `{"temp":23.5,"humidity":45.2,...}`
3. Check Serial Monitor for sensor read errors
4. Clear browser cache (Ctrl+Shift+Delete)
5. Try incognito/private browsing mode


**Problem: Dashboard loads but chart doesn't display**

**Solutions:**
1. Check Chart.js loaded (visit without internet to test CDN)
2. Verify history array has data
3. Open browser console for errors
4. Try refreshing page (F5)

---

### System Behavior Issues

**Problem: Fan rapidly cycles on/off**

**Cause:** Hysteresis too small or sensor in airflow

**Solutions:**
1. Increase HYSTERESIS constant to 2.0°C
2. Move sensor away from fan direct airflow
3. Increase update interval to 5 seconds
4. Add minimum runtime (don't allow OFF within 30s of ON)


**Problem: System crashes or resets randomly**

**Diagnostics:**
```cpp
// Serial Monitor shows:
Brownout detector was triggered
```

**Solutions:**
1. Use higher quality USB power supply (2A minimum)
2. Add 100µF capacitor near ESP32 VIN
3. Power fan from separate 5V supply
4. Check for loose wiring causing shorts
5. Verify relay coil current within ESP32 limits

## Applications & Relevance

**Autonomous Systems:**
This project demonstrates core principles used in autonomous platforms:

- **Sensor-Based Decision Making** - Environmental awareness and autonomous response
- **Remote Monitoring & Override** - Human-in-the-loop control capability
- **Fail-Safe Design** - Graceful degradation principles applicable to safety-critical systems
- **Thermal Management** - Direct relevance to electronics cooling in harsh environments

### Industrial Automation

**Process Control:**
- PLC-style threshold-based control logic
- Hysteresis implementation for actuator protection
- Remote monitoring and manual override
- SCADA system fundamentals

**Efficiency:**
- Demand-based cooling (only activated when needed)
- Configurable setpoints for optimization
- Historical data for efficient analysis
- Predictive maintenance via usage patterns

## Author

**Gavino Martinez**

Electrical Engineering Student | IoT Developer | Autonomous Systems

- GitHub: [@gavinomtz](https://github.com/gavinomtz)
- LinkedIn: [Gavino Martinez](https://linkedin.com/in/gavinomartinez)
- Email: gavinomtz@live.com

---

<div align="center">

**Built for Reliability • Designed for Autonomy • Engineered for Scale**

*Demonstrating IoT and embedded control principles for the next-generation*

## Demo Video

[![Watch Demo](media/demo-thumbnail.jpg)](https://www.youtube.com/watch?v=nSN4-HamR48)

**[Watch Full Demo on YouTube](https://www.youtube.com/watch?v=nSN4-HamR48)**

</div>

---

*This project was built to solve a real problem while demonstrating skills relevant to autonomous systems engineering. If you're working on similar challenges or want to discuss embedded systems, IoT architecture, or defense tech applications, let's connect!*