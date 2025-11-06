# Environmental Monitoring System

A resource-efficient microcontroller-based environmental sensor utilizing shift register architecture for optimized I/O management. Implements real-time temperature and humidity monitoring with LCD display through serial-to-parallel conversion.

## Overview

This system demonstrates advanced peripheral interfacing techniques by driving a 16x2 LCD display through a 74HC595 shift register, reducing GPIO requirements from 6+ pins to 3 pins while maintaining full display functionality. Integrates DHT11 environmental sensor for real-time atmospheric data acquisition and visualization.

## Core Features

* **Environmental Sensing** - DHT11-based temperature and humidity measurement
* **Shift Register Display Control** - 74HC595 serial-to-parallel LCD interface
* **Resource Optimization** - 50% reduction in GPIO pin usage vs. parallel LCD control
* **Real-Time Data Visualization** - 2-second refresh rate environmental monitoring
* **Modular Architecture** - Library-based peripheral abstraction for maintainability

## Hardware Architecture

| Component | Function | Interface |
|-----------|----------|-----------|
| Elegoo UNO R3 (ATmega328P) | Main MCU | - |
| DHT11 Sensor | Temperature/Humidity acquisition | Single-wire digital |
| 74HC595 Shift Register | Serial-to-parallel converter | SPI-like (3-wire) |
| 16x2 LCD (HD44780) | Visual output display | 8-bit parallel (via 74HC595) |

## Wiring Diagram

![Project Wiring](images/environment-display-shiftregister-wiring.png)

## System Operation

1. MCU polls DHT11 sensor at 2-second intervals
2. Temperature and humidity data acquired via single-wire protocol
3. Data formatted and transmitted to shift register via serial interface
4. 74HC595 converts serial data to 8-bit parallel output for LCD
5. LCD displays: `Temp: XX.X C` and `Humid: YY.Y %`

## Technical Skills Demonstrated

* **Serial Communication Protocols** - Shift register clocking and data serialization
* **GPIO Optimization** - Hardware multiplexing for pin conservation
* **Sensor Interfacing** - DHT single-wire protocol implementation
* **Display Driver Control** - HD44780 command set and timing management
* **Hardware Abstraction** - Library-based peripheral management
* **Real-Time Data Processing** - Sensor polling and display refresh synchronization

## Pin Configuration

### Standard LCD (6+ pins) vs. Shift Register (3 pins)

**Traditional Parallel LCD Interface:**
- RS, Enable, D4, D5, D6, D7 = **6 GPIO pins**

**This Implementation (via 74HC595):**
- Data, Clock, Latch = **3 GPIO pins** ✓

### Pinout

| Arduino Pin | Component/Function |
|-------------|-------------------|
| 4 | DHT11 data |
| 9 | Shift register (latch/SS) |

## Getting Started

### Prerequisites

* Arduino IDE 1.8+ or PlatformIO
* ShiftedLCD library
* DHT sensor library (Adafruit DHT or DHT sensor library)

### Installation

1. Clone this repository:
```bash
git clone https://github.com/gavinomtz/embedded-systems-projects/tree/main/environment-display-shiftregister.git
cd ./environmental-monitoring-system
```

2. Install required libraries via Arduino Library Manager:
   - **ShiftedLCD** by Sjorsvanleeuwen
   - **DHT sensor library** by Adafruit

3. Open the `.ino` file in Arduino IDE

4. Upload to your Arduino UNO R3

### Wiring Instructions

**DHT11 to Arduino:**
- VCC → 5V
- GND → GND
- DATA → Pin 4

**74HC595 to Arduino:**
- Controlled via ShiftedLCD library using Pin 9 (SS/Latch)
- Note: ShiftedLCD library handles SPI communication automatically

**74HC595 to LCD:**
- Q0-Q7 → LCD data pins (refer to ShiftedLCD wiring diagram)

## Usage

1. Power on the system
2. LCD initializes and displays environmental data
3. Readings automatically update every 2 seconds
4. Monitor real-time temperature (°C) and humidity (%)

## Design Rationale

### Why Shift Registers?

Traditional parallel LCD interfaces consume significant GPIO resources (6+ pins). By implementing a 74HC595 shift register:

- **Pin Conservation**: 3 pins vs 6+ pins (50% reduction)
- **Scalability**: Remaining GPIO available for additional sensors/peripherals
- **Industry Practice**: Common technique in resource-constrained embedded systems
- **Cost Efficiency**: Shift registers add minimal BOM cost vs benefit

### Applications

This architecture is relevant for:
- IoT sensor nodes with limited GPIO
- Multi-peripheral embedded systems
- Battery-powered devices (fewer active pins = lower power)
- Industrial monitoring systems

## Development Environment

* **Platform**: Arduino IDE / PlatformIO
* **Language**: C++ (Arduino framework)
* **Libraries**: ShiftedLCD.h, DHT.h
* **MCU**: ATmega328P (16MHz)

## Performance Metrics

| Metric | Value |
|--------|-------|
| Sensor update rate | 2 seconds |
| GPIO pins used | 2 (DHT + shift register SS) |
| Temperature range | 0-50°C (DHT11 spec) |
| Humidity range | 20-80% RH (DHT11 spec) |
| Display type | 16x2 character LCD |

## License

This project is open source and available under the [MIT License](LICENSE).

## Author

**Gavino Martinez**
- GitHub: [@gavinomtz](https://github.com/gavinomtz)
- LinkedIn: [Gavino Martinez](https://www.linkedin.com/in/gavinomartinez/)

## Acknowledgments

* Adafruit for DHT sensor libraries
* ShiftedLCD library contributors for shift register abstraction
* Arduino community for hardware reference implementations