#ifndef UTIL_C
#define UTIL_C

#define BEHAVIOURS_NUMBER 4
#define MAX_POWER 15
#define LEFT 0
#define RIGHT 1

bool g_isActive[BEHAVIOURS_NUMBER];
long g_behaviourOutputs[BEHAVIOURS_NUMBER][2];

int g_forageID = 0;
int g_followID = 1;
int g_avoidID = 2;
int g_observeID = 3;

void setMotors(long behaviourID, long leftMotor, long rightMotor) {
    g_behaviourOutputs[behaviourID][LEFT] = leftMotor;
    g_behaviourOutputs[behaviourID][RIGHT] = rightMotor;
}

void setStraight(long behaviourID) {
    g_behaviourOutputs[behaviourID][LEFT] = MAX_POWER;
    g_behaviourOutputs[behaviourID][RIGHT] = MAX_POWER;
}

void setStop(long behaviourID) {
    g_behaviourOutputs[behaviourID][LEFT] = 0;
    g_behaviourOutputs[behaviourID][RIGHT] = 0;
}

void setActive(long behaviourID) {
    g_isActive[behaviourID] = true;
}

void setInactive(long behaviourID) {
    g_isActive[behaviourID] = false;
}

bool isActive(long behaviourID) {
    return g_isActive[behaviourID];
}

// offset should never be less than -size
long offsettedCyclicIndex(long index, long offset, long size) {
    return (index + offset + size) % size;
}

void playTones(int number) {
    for(int k = 0; k < number; k++) {
        playTone(400, 2);
        delay(20);
    }
}

#endif UTIL_C
