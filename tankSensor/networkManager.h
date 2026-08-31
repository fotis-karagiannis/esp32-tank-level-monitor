#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

/**
  Library that handles the application network functions
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

extern WebServer server;
extern DNSServer dns;

extern String ssid;
extern bool   portalActive;

void loadCredentials();
void saveCredentials(const String& s, const String& p);

void initNetwork();
bool tryConnect();
void startCaptivePortal();
void beginServer();
void handleNetworkLoop();
void resetWiFi();

#endif
