# 🔐 Smart Door Security — ESP32 Firmware

An ESP32-based smart door security system that monitors physical sensors and communicates with a backend server over MQTT via WiFi. Supports motion detection, vibration intrusion detection, ultrasonic proximity sensing, magnetic reed-switch door state, servo-based locking, and a time-aware status LED.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Hardware Requirements](#hardware-requirements)
- [Pin Assignments](#pin-assignments)
- [Dependencies](#dependencies)
- [Configuration](#configuration)
- [MQTT Topics](#mqtt-topics)
- [Module Breakdown](#module-breakdown)
- [Sensor Behaviour](#sensor-behaviour)
- [Actuator Behaviour](#actuator-behaviour)
- [Timing & Debounce](#timing--debounce)
- [Getting Started](#getting-started)
- [Notes & Known Limitations](#notes--known-limitations)

---

## Architecture Overview

```
┌─────────────────────────────────┐
│         ESP32 Firmware          │
│                                 │
│  ┌──────────┐  ┌─────────────┐  │
│  │ sensors  │  │  actuators  │  │
│  └────┬─────┘  └──────┬──────┘  │
│       │               │         │
│  ┌────▼───────────────▼──────┐  │
│  │        main.cpp           │  │
│  │  (event loop + debounce)  │  │
│  └────────────┬──────────────┘  │
│               │                 │
│  ┌────────────▼──────────────┐  │
│  │      mqtt_handler         │  │
│  │  (WiFi + PubSubClient)    │  │
│  └────────────┬──────────────┘  │
└───────────────┼─────────────────┘
                │ MQTT over TCP
        ┌───────▼────────┐
        │  Mosquitto /   │
        │  Backend (Go)  │
        └────────────────┘
```

Sensors publish events to the backend; the backend sends commands back to control the servo and LED.

---

## Hardware Requirements

| Component | Model | Role |
|---|---|---|
| Microcontroller | ESP32 (any variant) | Main controller |
| PIR Sensor | HC-SR501 | Motion detection |
| Vibration Sensor | 801S | Intrusion/knock detection |
| Ultrasonic Sensor | HC-SR04 | Distance / proximity detection |
| Hall Effect Sensor | Generic | Magnetic field detection *(reserved)* |
| Magnetic Reed Switch | Generic | Door open/closed state |
| Servo Motor | SG90 | Door locking mechanism |
| Status LED | Built-in (GPIO 2) | Visual indicator |

---

## Pin Assignments

| GPIO | Role | Notes |
|---|---|---|
| 13 | PIR sensor | Active HIGH |
| 14 | Vibration sensor | Active HIGH |
| 5 | Ultrasonic TRIG | Output |
| 18 | Ultrasonic ECHO | Input |
| 25 | Hall effect sensor | Reserved – not wired in current circuit |
| 32 | Magnetic reed switch | `INPUT_PULLUP`; HIGH = door open |
| 4 | Servo motor (PWM) | SG90 via ESP32Servo |
| 2 | Status LED | Built-in LED |

---

## Dependencies

Install the following libraries via the Arduino Library Manager or PlatformIO:

| Library | Purpose |
|---|---|
| `PubSubClient` | MQTT client |
| `ESP32Servo` | Servo control on ESP32 PWM |
| `WiFi` *(built-in)* | WiFi connectivity |
| `time.h` *(built-in)* | NTP time sync |

---

## Configuration

All configuration lives in `config.h`. Edit this file before flashing:

```cpp
// WiFi
#define WIFI_SSID       "your-network-name"
#define WIFI_PASSWORD   "your-password"

// MQTT Broker (IP of the machine running Mosquitto)
#define MQTT_BROKER     "192.168.x.x"
#define MQTT_PORT       1883

// Timezone (IST = UTC+5:30 = 19800 s)
#define GMT_OFFSET_SEC  19800
```

> **Security note:** `config.h` currently stores credentials in plaintext. For production use, consider storing secrets in NVS (non-volatile storage) or using a provisioning flow.

---

## MQTT Topics

### ESP32 → Backend (Published)

| Topic | Payload | Description |
|---|---|---|
| `home/door/pir` | `DETECTED` | PIR motion event |
| `home/door/vibration` | `INTRUSION` | Vibration/knock event |
| `home/door/proximity` | `DETECTED` | Object closer than 10 cm |
| `home/door/ultrasonic` | `"85.5"` (float string) | Raw distance reading in cm |
| `home/door/hall` | `DETECTED` | Hall sensor event *(disabled in code)* |
| `home/door/magnetic/open` | *(empty)* | Door transitioned to OPEN |
| `home/door/magnetic/closed` | *(empty)* | Door transitioned to CLOSED |
| `home/door/motor` | `"55"` (int string) | Servo position in degrees |

### Backend → ESP32 (Subscribed)

| Topic | Payload | Action |
|---|---|---|
| `home/door/servo` | `UNLOCK` | Rotate servo to 55° (unlocked) |
| `home/door/servo` | `LOCK` | Rotate servo to 0° (locked) |
| `home/door/proximity_alert` | `VISITOR_NEAR` | Flash status LED (evening hours only) |

---

## Module Breakdown

### `config.h`
Central configuration file. Defines all GPIO pins, MQTT credentials, NTP settings, topic strings, servo positions, and timing constants. **Only file you need to edit before flashing.**

### `sensors.h` / `sensors.cpp`
Provides clean sensor-reading functions:
- `sensorsInit()` — configures all GPIO directions
- `pirDetected()` — reads PIR state
- `vibrationDetected()` — counts signal bounces in a 200 ms window
- `ultrasonicReadCm()` — triggers HC-SR04 and returns distance in cm (`-1.0` on timeout)
- `hallDetected()` — reads Hall sensor
- `magneticDoorOpen()` — returns `true` when reed switch reads HIGH (door open)

### `actuators.h` / `actuators.cpp`
Controls output hardware:
- `actuatorsInit()` — attaches servo and configures LED pin
- `servoUnlock()` / `servoLock()` — move servo to configured angles
- `moveServoSmooth(angle)` — incremental movement (1° steps, 15 ms delay) to avoid jerk/noise
- `ledFlash(ms)` / `ledUpdate()` — non-blocking LED flash using `millis()`

### `mqtt_handler.h` / `mqtt_handler.cpp`
Handles all network communication:
- Blocking WiFi connect on startup; automatic reconnect in `mqttLoop()`
- NTP time sync (up to 5 s wait, retries in background)
- Non-blocking MQTT reconnect with configurable retry delay
- Incoming message callback dispatches servo commands and proximity LED alerts
- `mqttPublish(topic, payload)` — safe publish with connected-state guard

### `main.cpp`
The main event loop:
- Calls all subsystem `init()` functions in `setup()`
- Publishes initial door state and motor position on boot
- Per-iteration debounce for PIR, vibration, proximity, and Hall sensors
- Edge-detection for magnetic reed switch (only publishes on state change)
- Periodic ultrasonic reads and motor position reads

---

## Sensor Behaviour

### PIR (HC-SR501)
Triggers on HIGH output. Debounced to at most one MQTT publish every **3 seconds**.

### Vibration (801S)
Uses a **200 ms sliding window**: counts signal transitions (bounces). Publishes only if bounce count exceeds 500 in that window — filters noise while detecting real impacts.

### Ultrasonic (HC-SR04)
Read every **2 seconds**. Publishes:
1. Raw distance (cm) to `home/door/ultrasonic`
2. A proximity event to `home/door/proximity` if distance < **10 cm** (debounced to 5 s)

Returns `-1.0` on timeout (no object in range within ~500 cm).

### Magnetic Reed Switch
**Edge-triggered only** — publishes to either `magnetic/open` or `magnetic/closed` only when state changes. Initial state published on boot.

### Hall Effect Sensor
GPIO and debounce logic is present in `config.h` but the publishing call in `main.cpp` is commented out. Reserved for future use.

---

## Actuator Behaviour

### Servo (SG90)
| State | Angle |
|---|---|
| Locked | 0° |
| Unlocked | 55° |

Moves smoothly in 1° increments with a 15 ms delay per step. Motor position is read back and published every **10 seconds** if it has changed.

### Status LED
- Turned ON by `ledFlash()`, automatically turned OFF after the configured duration via `ledUpdate()` in the main loop.
- The proximity alert LED response is **gated to evening hours** (18:00–06:00 IST) using NTP-synced local time. Falls back to "evening = true" if NTP is unavailable.

---

## Timing & Debounce

| Constant | Value | Purpose |
|---|---|---|
| `DEBOUNCE_PIR` | 3000 ms | Min interval between PIR publishes |
| `DEBOUNCE_VIBRATION` | 3000 ms | Min interval between vibration publishes |
| `DEBOUNCE_PROXIMITY` | 5000 ms | Min interval between proximity publishes |
| `DEBOUNCE_HALL` | 3000 ms | Min interval between Hall publishes |
| `ULTRASONIC_INTERVAL` | 2000 ms | Ultrasonic read frequency |
| `MOTOR_READ_INTERVAL` | 10000 ms | Servo position read frequency |
| `LED_FLASH_DURATION` | 3000 ms | LED on-time for proximity alert |
| `MQTT_RECONNECT_DELAY` | 5000 ms | Retry interval on MQTT disconnect |
| `WIFI_RETRY_DELAY` | 500 ms | Poll interval during WiFi connect |

---

## Getting Started

1. **Clone / copy** the source files into a PlatformIO or Arduino IDE project.
2. **Install dependencies** listed above.
3. **Edit `config.h`** — set your WiFi credentials and MQTT broker IP.
4. **Wire up hardware** according to the pin assignment table.
5. **Flash** to the ESP32.
6. **Monitor** via Serial at 115200 baud — all events and MQTT traffic are logged.
7. **Start your backend** (Mosquitto + subscriber service) on the configured broker IP.

---

## Notes & Known Limitations

- **WiFi credentials are hardcoded** in `config.h`. Do not commit this file to a public repository.
- **Vibration threshold** (`bounceCount > 500` in 200 ms) may need tuning depending on the sensor module and mounting surface.
- **`pulseIn()` blocks** for up to 30 ms during each ultrasonic read. This is acceptable at a 2-second interval but means other sensors are not polled during that window.
- **`moveServoSmooth()` is blocking** — it uses `delay()` internally, so the main loop is paused while the servo moves (~825 ms for a full 55° sweep). This is a known trade-off for smooth motion.
- **Hall effect sensor** wiring and publishing are disabled in the current build. GPIO 25 is reserved.
- **MQTT QoS is 0** (fire-and-forget). Messages may be lost if the broker is temporarily unreachable.
- **NTP sync** requires internet access. On a local-only WiFi network, NTP will time out and the LED will default to always-evening mode.
