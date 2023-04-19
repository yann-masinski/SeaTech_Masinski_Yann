#include "RobotState.h"
#include "Robot.h"
#include "main.h"

#define true 1
#define false 0


extern unsigned char stateRobot;


void SetRobotState(unsigned char state) {
    stateRobot = state;
}

void SetRobotAutoControlState(unsigned char payload) {
    if (payload == 0) {
        robotState.automatique = 0;
    }
    if (payload == 1) {
        robotState.automatique = 1;
    }
}