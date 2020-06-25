#ifndef STANDARD_DEVIATION_CALIBRATION_C
#define STANDARD_DEVIATION_CALIBRATION_C

#include "../util.c"
#include "read_light_sensor.c"

#define STANDARD_DEVIATION_THRESHOLD 100

float getStandardDeviation() {
    float sum = 0;

    for (int k = 0; k < LIGHT_WINDOW_SIZE; ++k) {
        sum += lightSensorWindow[k];
    }

    float mean = sum / LIGHT_WINDOW_SIZE;

    float standardDeviation = 0;
    for (int k = 0; k < LIGHT_WINDOW_SIZE; ++k) {
        standardDeviation += pow(lightSensorWindow[k] - mean, 2);
    }

    return standardDeviation / LIGHT_WINDOW_SIZE;
}

bool standardDeviationIsOnLine() {
    float standardDeviation = getStandardDeviation();
    return standardDeviation < STANDARD_DEVIATION_THRESHOLD;
}

#endif STANDARD_DEVIATION_CALIBRATION_C
