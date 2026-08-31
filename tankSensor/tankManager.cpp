#include "tankManager.h"
#include "hardwareManager.h"
#include <Arduino.h>

float tankWidth  = 0.0f;
float tankLength = 0.0f;
float tankHeight = 0.0f;
float tankOffset = 0.0f;
bool tankConfigured = false;

Preferences tankPrefs;

const char* PREF_NAMESPACE_TANK   = "tank";
const char* PREF_KEY_WIDTH        = "width";
const char* PREF_KEY_LENGTH       = "length";
const char* PREF_KEY_HEIGHT       = "height";
const char* PREF_KEY_OFFSET       = "offset";

// Loads tank settings from prefs
void loadTankConfig() {
    tankPrefs.begin(PREF_NAMESPACE_TANK, true);
    tankWidth  = tankPrefs.getFloat(PREF_KEY_WIDTH, 0.0f);
    tankLength = tankPrefs.getFloat(PREF_KEY_LENGTH, 0.0f);
    tankHeight = tankPrefs.getFloat(PREF_KEY_HEIGHT, 0.0f);
    tankOffset = tankPrefs.getFloat(PREF_KEY_OFFSET, 0.0f);
    tankPrefs.end();

    tankConfigured = (tankWidth > 0.0f && tankLength > 0.0f && tankHeight > 0.0f);
}


// Saves tank settings to prefs
void saveTankConfig(float width, float length, float height, float offset) {
    tankPrefs.begin(PREF_NAMESPACE_TANK, false);
    tankPrefs.putFloat(PREF_KEY_WIDTH,  width);
    tankPrefs.putFloat(PREF_KEY_LENGTH, length);
    tankPrefs.putFloat(PREF_KEY_HEIGHT, height);
    tankPrefs.putFloat(PREF_KEY_OFFSET, offset);
    tankPrefs.end();

    tankWidth  = width;
    tankLength = length;
    tankHeight = height;
    tankOffset = offset;
    tankConfigured = true;
}

// Re-calculates tank data based on found distance
TankInfo computeTankInfo(float distance) {
    TankInfo info;

    info.width  = tankWidth;
    info.length = tankLength;
    info.height = tankHeight;
    info.offset = tankOffset;

    if (!tankConfigured || distance < 0) {
        info.liquidHeight = -1;
        info.liters = -1;
        info.capacityLiters = -1;
        return info;
    }

    float distanceFromTopToLiquid = distance + tankOffset;
    float liquidHeight = tankHeight - distanceFromTopToLiquid;

    if (liquidHeight < 0) liquidHeight = 0;
    if (liquidHeight > tankHeight) liquidHeight = tankHeight;

    float liters = (tankWidth * tankLength * liquidHeight) / 1000.0f;
    float capacityLiters = (tankWidth * tankLength * tankHeight) / 1000.0f;

    info.liquidHeight = liquidHeight;
    info.liters = liters;
    info.capacityLiters = capacityLiters;

    return info;
}

// Simple sort function
static void sortArray(float* a, int n) {
    for (int i = 1; i < n; i++) {
        float k = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > k) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = k;
    }
}

static float computeMedian(float* arr, int n) {
    if (n == 0) return -1;
    sortArray(arr, n);
    return arr[n / 2];
}

/**
    Measurement logic: To reduce the chance of faulty/noisy measurements, the app measures 10 times, sorts the found distances and selects the median value
*/
// Handles the tank measurement calculations
TankInfo takeMeasurement() {
    const int SAMPLE_COUNT = 10;
    const int SAMPLE_DELAY_MS = 60;

    float samples[SAMPLE_COUNT];
    int valid = 0;
    int invalidCount = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float d = getDistance();
        if (d >= 0) samples[valid++] = d;
        else invalidCount++;
        delay(SAMPLE_DELAY_MS);
    }

    if (valid == 0 || invalidCount > 5) {
        return computeTankInfo(-1);
    }

    float median = computeMedian(samples, valid);

    return computeTankInfo(median);
}
