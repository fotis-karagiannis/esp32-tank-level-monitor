#ifndef DASHBOARD_MANAGER_H
#define DASHBOARD_MANAGER_H

/**
  Library that handles the application dashboard
*/

#include <Arduino.h>
#include "tankManager.h"
#include "networkManager.h"

void initDashboard();
void handleDashboard();
void handleData();
void handleMeasurement();

#endif
