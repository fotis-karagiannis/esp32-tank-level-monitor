#ifndef TANK_MANAGER_H
#define TANK_MANAGER_H

/**
    Library that handles tank calculation related functions
*/

#include <Preferences.h>

extern float tankWidth;
extern float tankLength;
extern float tankHeight;
extern float tankOffset;
extern bool tankConfigured;

extern Preferences tankPrefs;

struct TankInfo {
    float width;
    float length;
    float height;
    float offset;

    float liquidHeight;
    float liters;
    float capacityLiters;
};

void loadTankConfig();
void saveTankConfig(float width, float length, float height, float offset);

TankInfo computeTankInfo(float distance);
TankInfo takeMeasurement();

#endif
