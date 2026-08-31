#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "hardwareManager.h"
#include "networkManager.h"
#include "tankManager.h"
#include "dashboardManager.h"
#include "webpages.h"

// Setup Function
void setup() {
    Serial.begin(115200);

    initHardware();
    initNetwork();

    if (!tryConnect()) {
        startCaptivePortal();
    } else {
        updateLCDInitialConnected();
        initDashboard();
        beginServer();
    }

    if (portalActive) updateLCDPortalActive();
    else if (ssid != "") updateLCDInitialConnected();
    else updateLCDInitialNoWiFi();
}

// Loop Function
void loop() {
    handleNetworkLoop();
    server.handleClient();
}
