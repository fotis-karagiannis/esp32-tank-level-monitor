#include "networkManager.h"
#include "hardwareManager.h"
#include "tankManager.h"
#include "webpages.h"
#include <Preferences.h>
#include <ESPmDNS.h>

WebServer server(80);
DNSServer dns;

String ssid = "";
String pass = "";
bool   portalActive = false;

void loadCredentials() {
    Preferences prefs;
    prefs.begin("wifi", true);
    ssid = prefs.getString("ssid", "");
    pass = prefs.getString("pass", "");
    prefs.end();
}

void saveCredentials(const String& s, const String& p) {
    Preferences prefs;
    prefs.begin("wifi", false);
    prefs.putString("ssid", s);
    prefs.putString("pass", p);
    prefs.end();

    ssid = s;
    pass = p;
}

void initNetwork() {
    loadCredentials();
    loadTankConfig();
}

// Attempt to connect to wi-fi using saved credentials
bool tryConnect() {
    if (ssid == "") return false;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (millis() - start < 8000) {
        if (WiFi.status() == WL_CONNECTED) {
            MDNS.begin("tanksensor");
            return true;
        }
        delay(200);
    }
    return false;
}


// Starts the captive portal
void startCaptivePortal() {
    portalActive = true;

    WiFi.mode(WIFI_AP);
    WiFi.softAP("TankSensorSetup");

    dns.start(53, "*", WiFi.softAPIP());

    server.on("/", []() {
        String options = "";
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++) {
            options += "<option>" + WiFi.SSID(i) + "</option>";
        }

        String page = wifiSetupPage;
        page.replace("%OPTIONS%", options);
        server.send(200, "text/html", page);
    });

    server.on("/save", HTTP_POST, []() {
        String newSSID = server.arg("ssid");
        String newPASS = server.arg("pass");

        if (newSSID == "" || newPASS == "") {
            server.send(400, "text/plain", "Invalid WiFi credentials");
            return;
        }

        saveCredentials(newSSID, newPASS);
        server.send(200, "text/html", wifiSavedPage);
        
        delay(1500);
        ESP.restart();
    });

    server.onNotFound([]() {
        server.sendHeader("Location", "/", true);
        server.send(302);
    });

    beginServer();
}

void beginServer() {
    server.begin();
}


// Simple network loop
void handleNetworkLoop() {
    if (portalActive) {
        dns.processNextRequest();
        blinkLED(1200); // slow blink when captive portal is on
    }

    handlePortalBeep();
    handleButtonHold();
}

// Reset wi-fi by deleting credentials and restarting
void resetWiFi() {
    Preferences prefs;
    prefs.begin("wifi", false);
    prefs.putString("ssid", "");
    prefs.putString("pass", "");
    prefs.end();

    server.stop();
    ESP.restart();
}
