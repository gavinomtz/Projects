# Arduino Keypad Servo Door Lock System

This project implements a password-protected door locking mechanism using an Elegoo UNO R3, a 4x4 membrane keypad, and a micro servo actuator.  
When the correct 4-digit PIN is entered on the keypad, the servo rotates to unlock the mechanism. Incorrect PIN entries trigger a buzzer and red indicator LED.

---

## Features

- 4-digit PIN code entry
- 4x4 matrix keypad for input
- Micro servo for lock mechanism actuation
- Visual + audible feedback (LED + buzzer)
- Lock resets automatically after delay
- Simple Arduino C++ based state logic

---

## Hardware Used

| Component | Purpose |
|----------|---------|
| Elegoo UNO R3 | main microcontroller |
| 4x4 membrane keypad | numeric input |
| SG90 micro servo | lock actuator |
| Red LED | access denied indicator |
| Green LED | access granted indicator |
| Piezo buzzer | alarm feedback |

---

## Code Overview

- PIN is defined in code (`String password = "1234";`)
- `#` = Enter/submit PIN
- `*` = clear/reset input
- Correct PIN → servo rotates to 90° (unlock)
- After 5s → returns to 0° (locked)
- Incorrect PIN → buzzer + red LED flash

---

## Skills Demonstrated

- Embedded C++ programming
- Matrix keypad scanning / data entry handling
- Actuator control (PWM servo)
- User authentication logic in firmware
- Hardware feedback loops (LED, buzzer)

---