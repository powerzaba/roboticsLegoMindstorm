#ifndef READ_LIGHT_SENSOR_C
#define READ_LIGHT_SENSOR_C

#include "../util.c"
#include "HitechnicColorSensor.h"

#define LIGHT_WINDOW_SIZE 5

long lightSensorWindow[LIGHT_WINDOW_SIZE];
long lightReadingIndex = 0;

float average(long a, long b, long c) {
    return (a + b + c) / 3;
}

long lightLastReadValue() {
    return lightSensorWindow[offsettedCyclicIndex(lightReadingIndex, -1, LIGHT_WINDOW_SIZE)];
}

void lightNewReading(float value) {
    lightSensorWindow[lightReadingIndex] = value;
    lightReadingIndex = offsettedCyclicIndex(lightReadingIndex, 1, LIGHT_WINDOW_SIZE);
}

task readLightSensor() {
    while (true) {
        long r, g, b, av;

        HTCS2readRawRGB(S3, true, r, g, b);
        av = average(r, g, b);

        displayTextLine(1, "Current light average: %d", av);
        lightNewReading(av);
    }
}

#endif READ_LIGHT_SENSOR_C
