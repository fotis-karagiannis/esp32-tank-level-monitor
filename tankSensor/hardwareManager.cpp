#include "hardwareManager.h"
#include "networkManager.h"
#include "tankManager.h"
#include <Wire.h>

// Pin definition
const int POWER_LED_PIN = 33;
const int LED_PIN       = 32;
const int BUTTON_PIN    = 18;
const int BUZZER_PIN    = 19;
const int TRIG_PIN      = 4;
const int ECHO_PIN      = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Measurement state
bool  measurementAttempted = false;
float lastMeasurement       = -1.0f;
int   lastValidCount        = 0;

// LED timing
const unsigned long FAST_BLINK = 150;

// Flag for fast led blinking
bool buttonHeldFastBlink = false;

// Initializes the hardware components
void initHardware() {
    pinMode(POWER_LED_PIN, OUTPUT);
    digitalWrite(POWER_LED_PIN, HIGH);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Wire.begin();
    lcd.init();
    lcd.backlight();
    enableLED(); //default led state is ON, let other handlers change it

    beep(200);
}

// Buzzer functions
void beep(unsigned long duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
}

void handlePortalBeep() {
    if (!portalActive) return;
    unsigned long now = millis();
    static unsigned long lastPortalBeep = 0;
    if (now - lastPortalBeep >= 2000) {
        lastPortalBeep = now;
        beep(80);
    }
}

// Led functions
void blinkLED(unsigned long interval) {
    static unsigned long prev = 0;
    static bool s = HIGH;   // LED starts ON by default

    if (millis() - prev >= interval) {
        prev = millis();
        s = !s;
        digitalWrite(LED_PIN, s);
    }
}


void blinkOnce() {
    digitalWrite(LED_PIN, HIGH);   // LED is ON by default
    delay(150);

    digitalWrite(LED_PIN, LOW);
    delay(150);

    digitalWrite(LED_PIN, HIGH);   // restore ON
}


void enableLED() {
    digitalWrite(LED_PIN, HIGH);
}

// LCD functions
void updateLCDInitialConnected() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected to:");
    lcd.setCursor(0, 1);
    lcd.print(ssid.substring(0, 16));
}

void updateLCDInitialNoWiFi() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi not found");
    lcd.setCursor(0, 1);
    lcd.print("Open setup portal");
}

void updateLCDPortalActive() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setup Mode");
    lcd.setCursor(0, 1);
    lcd.print("Connect via WiFi");
}

// Updates the LCD display
void updateLCDMeasurementDisplay(const TankInfo& info) {
    lcd.clear();

    if (!measurementAttempted) {
        if (portalActive) updateLCDPortalActive();
        else if (ssid != "") updateLCDInitialConnected();
        else updateLCDInitialNoWiFi();
        return;
    }

    if (info.liquidHeight < 0) {
        lcd.setCursor(0, 0);
        lcd.print("Measurement:");
        lcd.setCursor(0, 1);
        lcd.print("INVALID");
        return;
    }

    float distance = info.offset + (info.height - info.liquidHeight);

    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    lcd.setCursor(0, 1);

    char buf[17];
    snprintf(buf, sizeof(buf), "%.2f cm", distance);
    lcd.print(buf);

    delay(800);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Volume:");

    char vbuf[17];
    snprintf(vbuf, sizeof(vbuf), "%.2f L", info.liters);
    lcd.setCursor(0, 1);
    lcd.print(vbuf);
}

void runSamplingAnimation() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sampling...");

    for (int i = 0; i <= 16; i++) {
        lcd.setCursor(0, 1);
        int filled = i;
        for (int j = 0; j < 16; j++) {
            lcd.print(j < filled ? char(255) : ' ');
        }
        delay(120);
    }
}

// Handles button presses
void handleButtonHold() {
    static unsigned long buttonPressStart = 0;
    static bool buttonHeld = false;
    static int lastReading = HIGH;
    static unsigned long lastDebounceTime = 0;
    static bool longPressHandled = false;

    // Local fast-blink timer
    static unsigned long fastBlinkPrev = 0;
    static bool fastBlinkState = LOW;

    const unsigned long DEBOUNCE_MS   = 50;
    const unsigned long LONG_PRESS_MS = 5000;

    int reading = digitalRead(BUTTON_PIN);

    if (reading != lastReading)
        lastDebounceTime = millis();

    if (millis() - lastDebounceTime > DEBOUNCE_MS) {

        // Button pressed
        if (reading == LOW && !buttonHeld) {
            buttonPressStart = millis();
            buttonHeld = true;
            longPressHandled = false;

            // Start fast blink immediately
            buttonHeldFastBlink = true;

            fastBlinkPrev = millis();
            fastBlinkState = LOW;
        }

        // Button released
        else if (reading == HIGH && buttonHeld) {
            unsigned long held = millis() - buttonPressStart;
            buttonHeld = false;

            // Stop fast blink
            buttonHeldFastBlink = false;

            // Return LED to enabled
            enableLED();

            // Short press (measurement)
            if (!longPressHandled && held > 50 && held < LONG_PRESS_MS) {

                if (!tankConfigured) {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Please visit");
                    lcd.setCursor(0, 1);
                    lcd.print("tanksensor.local!");
                    return;
                }

                runSamplingAnimation();
                TankInfo info = takeMeasurement();

                measurementAttempted = true;
                if (info.liquidHeight < 0) {
                    lastMeasurement = -1;
                    lastValidCount  = 0;
                } else {
                    lastMeasurement = info.offset + (info.height - info.liquidHeight);
                    lastValidCount  = 10;
                }

                updateLCDMeasurementDisplay(info);
                beep(120);
                blinkOnce();
            }
        }

        // Holding fast blink
        if (buttonHeld && !longPressHandled) {
            if (millis() - fastBlinkPrev >= FAST_BLINK) {
                fastBlinkPrev = millis();
                fastBlinkState = !fastBlinkState;
            }
            
            digitalWrite(LED_PIN, fastBlinkState ? HIGH : LOW);
        }


        // Long press trigger
        if (buttonHeld && !longPressHandled &&
            millis() - buttonPressStart >= LONG_PRESS_MS) {

            longPressHandled = true;

            // Stop fast blink during reset
            buttonHeldFastBlink = false;

            resetWiFi();
        }

    }

    lastReading = reading;
}

// Sensor handling
float getDistance() {
    delay(50);
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    unsigned long dur = pulseIn(ECHO_PIN, HIGH, 20000);
    if (dur == 0) return -1;

    float d = (dur * 0.0343f) / 2.0f;
    if (d < 2.0f) return -1;

    return d;
}
