#ifndef  FOLLOW_LINE_C
#define  FOLLOW_LINE_C

#include "../util.c"
#include "read_light_sensor.c"
#include "standard_deviation_calibration.c"

#define MAX_WHITE_READINGS 500
#define ON_LINE_THRESHOLD 20
#define STEPBACK_FL 600

bool g_followFoundLine = false;
bool g_looseTrack;
bool g_lineOnRight = true;
long g_onLine = 0;
long g_offLine = 0;

const float primaryMotor[3] = {1.0, 1.0, 0.8};
const float secondaryMotor[3] = {0.5, 0.0, -0.6};
const long countThreshold[2] = {5, 35};

void followLooseTrack() {
    g_followFoundLine = false;
    g_looseTrack = true;
}

void followCountOnOffLine() {
    bool isOnLine = standardDeviationIsOnLine();

    if (isOnLine) {
        g_onLine++;
        g_offLine = 0;
    } else {
        g_onLine = 0;
        g_offLine++;
    }
}

long followGetPrimaryPower(long count) {
    long primaryPower;

    if (count <= countThreshold[0]) {
        primaryPower = MAX_POWER * primaryMotor[0];
    } else if (count <= countThreshold[1]) {
        primaryPower = MAX_POWER * primaryMotor[1];
    } else {
        primaryPower = MAX_POWER * primaryMotor[2];
    }

    return primaryPower;
}

long followGetSecondaryPower(long count) {
    long secondaryPower;

    if (count <= countThreshold[0]) {
        secondaryPower = MAX_POWER * secondaryMotor[0];
    } else if (count <= countThreshold[1]) {
        secondaryPower = MAX_POWER * secondaryMotor[1];
    } else {
        secondaryPower = MAX_POWER * secondaryMotor[2];
    }

    return secondaryPower;
}

enum FollowStates {
    LINE_SEARCHING,
    LINE_STEPBACK,
    LINE_FOLLOWING
};

task followBehaviour() {
    enum FollowStates state = LINE_SEARCHING;
    g_looseTrack = false;

    while (true) {
        followCountOnOffLine();

        switch (state) {
            case LINE_SEARCHING:
                setInactive(g_followID);

                if (g_onLine > ON_LINE_THRESHOLD) {
                    playTones(8);
                    state = LINE_STEPBACK;
                    g_followFoundLine = true;
                }

                break;
            case LINE_STEPBACK:
                setActive(g_followID);
                setMotors(g_followID, -MAX_POWER, -MAX_POWER);

                delay(STEPBACK_FL);
                while(g_onLine);

                state = LINE_FOLLOWING;

                break;
            case LINE_FOLLOWING:
                setActive(g_followID);

                if (g_onLine > 0) {
                    if (g_lineOnRight) {
                        setMotors(g_followID, followGetSecondaryPower(g_onLine), followGetPrimaryPower(g_onLine));
                    } else {
                        setMotors(g_followID, followGetPrimaryPower(g_onLine), followGetSecondaryPower(g_onLine));
                    }
                } else {
                    if (g_lineOnRight) {
                        setMotors(g_followID, followGetPrimaryPower(g_offLine), followGetSecondaryPower(g_offLine));
                    } else {
                        setMotors(g_followID, followGetSecondaryPower(g_offLine), followGetPrimaryPower(g_offLine));
                    }
                }

                if (g_offLine >= MAX_WHITE_READINGS || g_looseTrack) {
                    state = LINE_SEARCHING;
                    g_followFoundLine = false;
                    g_looseTrack = false;
                }

                break;
        }
    }
}

#endif FOLLOW_LINE_C
