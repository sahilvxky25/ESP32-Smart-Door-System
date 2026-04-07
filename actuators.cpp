#include "actuators.h"
#include "config.h"
#include <ESP32Servo.h>

// ──────────────────────────────────────────────
//  Internals
// ──────────────────────────────────────────────
static Servo doorServo;
static bool          ledFlashing   = false;
static unsigned long ledOffTime    = 0;

// Smooth motion parameters
static const int STEP_DELAY = 15;   // ms delay → controls speed
static const int STEP_SIZE  = 1;    // degree step → controls smoothness

// ──────────────────────────────────────────────
//  Initialisation
// ──────────────────────────────────────────────
void actuatorsInit() {
    doorServo.attach(PIN_SERVO);
    doorServo.write(SERVO_LOCKED);   // start locked

    pinMode(PIN_STATUS_LED, OUTPUT);
    digitalWrite(PIN_STATUS_LED, LOW);
}

// ──────────────────────────────────────────────
//  Smooth Servo Movement Function
// ──────────────────────────────────────────────
void moveServoSmooth(int targetAngle) {
    int currentAngle = doorServo.read();

    if (currentAngle < targetAngle) {
        for (int pos = currentAngle; pos <= targetAngle; pos += STEP_SIZE) {
            doorServo.write(pos);
            delay(STEP_DELAY);
        }
    } else {
        for (int pos = currentAngle; pos >= targetAngle; pos -= STEP_SIZE) {
            doorServo.write(pos); 
            delay(STEP_DELAY);
        }
    }
}

// ──────────────────────────────────────────────
//  Servo
// ──────────────────────────────────────────────
void servoUnlock() {
    Serial.println("[Actuator] Servo → UNLOCKED (90°)");
    moveServoSmooth(SERVO_UNLOCKED);
}

void servoLock() {
    Serial.println("[Actuator] Servo → LOCKED (0°)");
    moveServoSmooth(SERVO_LOCKED);
}

int servoRead() {
    return doorServo.read();
}

// ──────────────────────────────────────────────
//  Status LED
// ──────────────────────────────────────────────
void ledOn() {
    digitalWrite(PIN_STATUS_LED, HIGH);
}

void ledOff() {
    digitalWrite(PIN_STATUS_LED, LOW);
    ledFlashing = false;
}

void ledFlash(unsigned long durationMs) {
    ledOn();
    ledFlashing = true;
    ledOffTime  = millis() + durationMs;
}

void ledUpdate() {
    if (ledFlashing && millis() >= ledOffTime) {
        ledOff();
    }
}