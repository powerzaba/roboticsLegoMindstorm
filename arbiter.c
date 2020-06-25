#ifndef ARBITER_C
#define ARBITER_C

#include "util.c"
#include "forage.c"
#include "follow_line/follow_line.c"
#include "avoid/avoid.c"
#include "observe/observe.c"

task arbiter() {
	for(long k = 0; k < BEHAVIOURS_NUMBER; k++) {
		g_isActive[k] = false;
		g_behaviourOutputs[k][LEFT] = 0;
        g_behaviourOutputs[k][RIGHT] = 0;
    }

    startTask(readLightSensor);
    //startTask(readSonarSensors);
	delay(1000);
	startTask(forageBehaviour);
	delay(100);
	startTask(followBehaviour);
	delay(100);
	//startTask(avoidBehaviour);
	delay(100);
	startTask(observeBehaviour);

	while(true) {
		for(long behaviourID = BEHAVIOURS_NUMBER - 1; behaviourID >= 0; behaviourID--) {
			if(isActive(behaviourID)) {
				setMotorSpeed(mLeft, g_behaviourOutputs[behaviourID][LEFT]);
				setMotorSpeed(mRight, g_behaviourOutputs[behaviourID][RIGHT]);
				break;
			}
		}
	}
}

#endif ARBITER_C
