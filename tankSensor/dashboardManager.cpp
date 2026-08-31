#include "dashboardManager.h"
#include "hardwareManager.h"
#include "tankManager.h"
#include "webpages.h"
#include "networkManager.h"
#include <math.h>

// Initializes the application dashboard
void initDashboard() {
    server.on("/",        handleDashboard);
    server.on("/data",    handleData);
    server.on("/measure", HTTP_POST, handleMeasurement);

    server.on("/dimensions", []() {
        String page = dimensionsPage;
        page.replace("%WIDTH%",  String((int)round(tankWidth)));
        page.replace("%LENGTH%", String((int)round(tankLength)));
        page.replace("%HEIGHT%", String((int)round(tankHeight)));
        page.replace("%OFFSET%", String((int)round(tankOffset)));
        server.send(200, "text/html", page);
    });

    server.on("/saveDimensions", HTTP_POST, []() {
        float width  = server.arg("width").toFloat();
        float length = server.arg("length").toFloat();
        float height = server.arg("height").toFloat();
        float offset = server.arg("offset").toFloat();

        if (width <= 0 || length <= 0 || height <= 0 || offset < 0) {
            server.send(400, "text/plain", "Invalid dimensions");
            return;
        }

        saveTankConfig(width, length, height, offset);
        server.send(200, "text/html", wifiSavedPage);
    });
}


// Handles the measurement operation
void handleMeasurement() {
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

    server.send(200, "text/plain", "OK");
}

void handleDashboard() {
    server.send(200, "text/html", dashboardPage);
}


// Handle the data to be displayed on the dashboard
void handleData() {
    TankInfo info = computeTankInfo(lastMeasurement);

    String text;
    String litersText = "";
    float litersNum      = info.liters;
    float capacityLiters = info.capacityLiters;

    if (!measurementAttempted)
        text = "NO_MEASUREMENT";
    else if (lastMeasurement < 0)
        text = "INVALID";
    else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f cm", lastMeasurement);
        text = buf;

        if (tankConfigured && litersNum >= 0) {
            char lbuf[32];
            snprintf(lbuf, sizeof(lbuf), "%.2f L", litersNum);
            litersText = lbuf;
        }
    }

    String json = "{";
    json += "\"text\":\"" + text + "\",";
    json += "\"liters\":\"" + litersText + "\",";
    json += "\"liters_num\":" + String(litersNum, 2) + ",";
    json += "\"capacity_liters\":" + String(capacityLiters, 2) + ",";
    json += "\"tankConfigured\":" + String(tankConfigured ? "true" : "false") + ",";
    json += "\"width\":" + String(info.width, 2) + ",";
    json += "\"length\":" + String(info.length, 2) + ",";
    json += "\"height\":" + String(info.height, 2) + ",";
    json += "\"offset\":" + String(info.offset, 2) + ",";
    json += "\"valid\":" + String(lastValidCount);
    json += "}";

    server.send(200, "application/json", json);
}
