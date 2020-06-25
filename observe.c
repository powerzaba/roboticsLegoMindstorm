#ifndef OBSERVE_C
#define OBSERVE_C

#include "util.c"
#include "follow_line/follow_line.c"

#define GYRO_THRESHOLD 45
#define MIN_ONE_LAP_TIME 120000
#define MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS 2
#define TARGET_TURN_BACK 350
#define THRESHOLD_SLOPE 0.08
#define THRESHOLD_BIAS 240
#define ADJUSTMENT_TIME 5000

long obsLongestTimesStraight[MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS];
long obsCountLongestStraightOccurrence;
long obsCurrentPath;

// Returns true if updated, false otherwise.
bool obsUpdateLongestPaths(long path) {
    int smallestIndex = 0;

    for (int k = 1; k < MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS; k++) {
        if (obsLongestTimesStraight[k] < obsLongestTimesStraight[smallestIndex]) {
            smallestIndex = k;
        }
    }

    if (obsLongestTimesStraight[smallestIndex] <= path) {
        obsLongestTimesStraight[smallestIndex] = path;
        return true;
    }

    return false;
}

float obsGetThreshold(long path) {
    return THRESHOLD_SLOPE * path - THRESHOLD_BIAS;
}

bool obsIsInThreshold(float origin, float value, float threshold) {
    return abs(origin - value) < threshold;
}

bool obsAllInThreshold(float average, float threshold) {
    for (int k = 0; k < MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS; k++) {
        if (!obsIsInThreshold(average, obsLongestTimesStraight[k], threshold)) {
            return false;
        }
    }

    return true;
}

bool obsArePathsSimilar() {
    float sum = 0;

    for (int k = 0; k < MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS; k++) {
        sum += obsLongestTimesStraight[k];
    }

    float average = sum / MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS;

    return obsAllInThreshold(average, obsGetThreshold(average));
}

enum ObserveState {
    OBSERVE_WAITING,
    OBSERVE_OBSERVING,
    OBSERVE_CHANGE_DIRECTION,
    OBSERVE_GO_MID
};

task observeBehaviour() {
    enum ObserveState state = OBSERVE_WAITING;

    while (true) {
        displayTextLine(2, "Gyro heading: %d", getGyroHeading(sGyro));
        displayTextLine(3, "TIme length current Path: %d", obsCurrentPath);

        switch (state) {
            case OBSERVE_WAITING:
                setInactive(g_observeID);

                if (g_followFoundLine) {
                    state = OBSERVE_OBSERVING;
                    resetGyro(sGyro);
                    clearTimer(T1);
                    clearTimer(T2);

                    for (int k = 0; k < MINIMUM_LONGEST_STRAIGHT_ENCOUNTERS; k++) {
                        obsLongestTimesStraight[k] = -1;
                    }

                    obsCountLongestStraightOccurrence = 0;
                }

                break;
            case OBSERVE_OBSERVING:
                setInactive(g_observeID);

                if (!g_followFoundLine) {
                    state = OBSERVE_WAITING;
                    break;
                }

                if (abs(getGyroHeading(sGyro)) > GYRO_THRESHOLD) {
                    playTones(1);

                    obsCurrentPath = time1[T1];

                    if (obsUpdateLongestPaths(obsCurrentPath)
                        && time1[T2] > MIN_ONE_LAP_TIME
                        && obsArePathsSimilar()) {
                        state = OBSERVE_CHANGE_DIRECTION;
                        resetMotorEncoder(mRight);
                    }

                    clearTimer(T1);
                    resetGyro(sGyro);
                }

                break;
            case OBSERVE_CHANGE_DIRECTION:
                setActive(g_observeID);
                setMotors(g_observeID, -MAX_POWER, MAX_POWER);

                if (getMotorEncoder(mRight) >= TARGET_TURN_BACK) {
                    g_lineOnRight = false;
                    state = OBSERVE_GO_MID;
                }

                break;
            case OBSERVE_GO_MID:
                playTones(12);
                setInactive(g_observeID);

                delay((obsCurrentPath / 2) + ADJUSTMENT_TIME);

                setActive(g_observeID);
                setStop(g_observeID);
                playTones(12);

                return;
        }
    }
}

#endif OBSERVE_C
