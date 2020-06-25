#ifndef AVOID_C
#define AVOID_C

#include "../util.c"
#include "../follow_line/follow_line.c"
#include "read_sonar_sensors.c"

#define SECONDARY_MOTOR_CURVE_WEIGHT 0.7
#define OBSTACLE_DISTANCE_THRESHOLD 8
#define TARGET_STEPBACK -50
#define TARGET_ROTATE 130
#define AVOID_TIME 4000

void avoidResetEncoders() {
    resetMotorEncoder(mLeft);
    resetMotorEncoder(mRight);
}

enum AvoidStateS {
    AVOID_DETECT,
    AVOID_STEPBACK,
    AVOID_ROTATE,
    AVOID_CURVE,
};

task avoidBehaviour() {
    enum AvoidStateS state = AVOID_DETECT;

    while (true) {
        switch (state) {
            case AVOID_DETECT:
                setInactive(g_avoidID);

                if (sonarGetConservativeValue() < OBSTACLE_DISTANCE_THRESHOLD) {
                    state = AVOID_STEPBACK;
                    avoidResetEncoders();
                    playTones(8);
                }

                break;
            case AVOID_STEPBACK:
                setActive(g_avoidID);
                setMotors(g_avoidID, -MAX_POWER, -MAX_POWER);

                if (getMotorEncoder(mRight) <= TARGET_STEPBACK) {
                    state = AVOID_ROTATE;
                    avoidResetEncoders();
                }

                break;
            case AVOID_ROTATE:
                setActive(g_avoidID);
                setMotors(g_avoidID, -MAX_POWER, MAX_POWER);

                if (getMotorEncoder(mRight) >= TARGET_ROTATE) {
                    state = AVOID_CURVE;
                    followLooseTrack();
                }

                break;
            case AVOID_CURVE:
                setActive(g_avoidID);
                setMotors(g_avoidID, MAX_POWER, MAX_POWER * SECONDARY_MOTOR_CURVE_WEIGHT);

                if (sonarGetConservativeValue() < OBSTACLE_DISTANCE_THRESHOLD) {
                    state = AVOID_STEPBACK;
                    avoidResetEncoders();
                }

                if (g_followFoundLine) {
                    state = AVOID_DETECT;
                }

                break;
        }
    }
}

#endif AVOID_C
