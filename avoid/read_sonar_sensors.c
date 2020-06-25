#ifndef READ_SONAR_SENSORS_C
#define READ_SONAR_SENSORS_C

#include "../util.c"

long lastLeftSonarReading;
long lastRightSonarReading;

float sonarGetConservativeValue() {
    return lastLeftSonarReading < lastRightSonarReading ? lastLeftSonarReading : lastRightSonarReading;
}

task readSonarSensors() {
    while (true) {
        delay(50);
        lastLeftSonarReading = getUSDistance(sLeft);
        lastRightSonarReading = getUSDistance(sRight);
    }
}

#endif READ_LIGHT_SENSOT_C
