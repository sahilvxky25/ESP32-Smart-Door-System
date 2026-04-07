#ifndef CONFIG_H
#define CONFIG_H

// ──────────────────────────────────────────────
//  WiFi credentials  (change these!)
// ──────────────────────────────────────────────
#define WIFI_SSID     "OPPO K13 5G 8F3A"
#define WIFI_PASSWORD "csgy6924"

// ──────────────────────────────────────────────
//  MQTT broker  (matches backend .env MQTT_BROKER)
// ──────────────────────────────────────────────
#define MQTT_BROKER   "172.23.26.147"   // IP of machine running Mosquitto
#define MQTT_PORT     1883
#define MQTT_CLIENT_ID "smart-door-esp32"

// ──────────────────────────────────────────────
//  NTP time sync (for evening-only LED logic)
// ──────────────────────────────────────────────
#define NTP_SERVER        "time.google.com"
#define GMT_OFFSET_SEC    19800          // IST = UTC+5:30 = 19800 seconds
#define DAYLIGHT_OFFSET   0
#define EVENING_HOUR      18             // LED activates from 6 PM onwards
#define MORNING_HOUR       6             // LED deactivates from 6 AM onwards

// ──────────────────────────────────────────────
//  MQTT topics  (must match backend subscriber.go)
// ──────────────────────────────────────────────
// ESP32 → Backend (publish)
#define TOPIC_PIR         "home/door/pir"
#define TOPIC_VIBRATION   "home/door/vibration"
#define TOPIC_PROXIMITY   "home/door/proximity"
#define TOPIC_ULTRASONIC  "home/door/ultrasonic"
#define TOPIC_HALL        "home/door/hall"
//#define TOPIC_MAGNETIC    "home/door/magnetic"
#define TOPIC_MOTOR       "home/door/motor"
//#define TOPIC_INTRUSION   "home/door/intrusion"
#define TOPIC_MAGNETIC_OPEN   "home/door/magnetic/open"
#define TOPIC_MAGNETIC_CLOSED "home/door/magnetic/closed"

// Backend → ESP32 (subscribe)
#define TOPIC_SERVO           "home/door/servo"
#define TOPIC_PROXIMITY_ALERT "home/door/proximity_alert"

// ──────────────────────────────────────────────
//  GPIO pin assignments
// ──────────────────────────────────────────────
// Sensors
#define PIN_PIR             13    // PIR motion sensor (HC-SR501), active HIGH
#define PIN_VIBRATION       14    // Vibration sensor (801S), active HIGH
#define PIN_ULTRASONIC_TRIG 5    // Ultrasonic trigger (matches schematic IO23)
#define PIN_ULTRASONIC_ECHO 18    // Ultrasonic echo   (matches schematic IO22)
#define PIN_HALL            25    // Hall effect sensor (reserved – not currently wired)
#define PIN_MAGNETIC        32    // Magnetic reed switch (door open/closed)

// Actuators
#define PIN_SERVO            4    // SG90 servo motor (PWM)
#define PIN_STATUS_LED       2    // Built-in LED

// ──────────────────────────────────────────────
//  Servo positions (degrees)
// ──────────────────────────────────────────────
#define SERVO_LOCKED       0    // 0° = locked
#define SERVO_UNLOCKED     55   // 55° = unlocked

// ──────────────────────────────────────────────
//  Ultrasonic proximity thresholds (cm)
// ──────────────────────────────────────────────
#define PROXIMITY_THRESHOLD_CM  10.0   // < 10 cm → publish "DETECTED" to proximity topic

// ──────────────────────────────────────────────
//  Timing & debounce (milliseconds)
// ──────────────────────────────────────────────
#define DEBOUNCE_PIR          3000    // 3 s between PIR triggers
#define DEBOUNCE_VIBRATION    3000    // 3 s between vibration triggers
#define DEBOUNCE_PROXIMITY    5000    // 5 s between ultrasonic proximity triggers
#define DEBOUNCE_HALL         3000    // 3 s between hall triggers
#define ULTRASONIC_INTERVAL   2000    // read ultrasonic every 2 s
#define MOTOR_READ_INTERVAL   10000    // read servo position every 10 s
#define LED_FLASH_DURATION    3000    // proximity-alert LED on-time
#define MQTT_RECONNECT_DELAY  5000    // retry MQTT every 5 s
#define WIFI_RETRY_DELAY      500     // ms between WiFi connect checks

#endif // CONFIG_H
