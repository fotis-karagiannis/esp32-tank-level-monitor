#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

/**
  Library that handles hardware-related functions
*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "tankManager.h"

// Measurement state (global, shared)
extern bool  measurementAttempted;
extern float lastMeasurement;
extern int   lastValidCount;

// WiFi / portal state (from networkManager)
extern bool   portalActive;
extern String ssid;

// Tank config flag (from tankManager)
extern bool tankConfigured;

// Hardware lifecycle
void initHardware();

// Buzzer
void beep(unsigned long duration);
void handlePortalBeep();

// LED
void blinkLED(unsigned long interval);
void blinkOnce();
void enableLED();

// LCD
void updateLCDInitialConnected();
void updateLCDInitialNoWiFi();
void updateLCDPortalActive();
void updateLCDMeasurementDisplay(const TankInfo& info);
void runSamplingAnimation();

// Button
void handleButtonHold();

// Ultrasonic
float getDistance();

#endif
