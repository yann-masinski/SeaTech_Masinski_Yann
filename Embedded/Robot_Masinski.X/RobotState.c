#include "RobotState.h"
#include "Robot.h"
#include "main.h"

#define true 1
#define false 0


extern unsigned char stateRobot;


void SetRobotState(unsigned char state) {
    if (robotState.automatique == 0)
    {
        stateRobot = state;
    }
}

void SetRobotAutoControlState(unsigned char state) {
    if (state == 0) {
        robotState.automatique = 0;
    }
    else
    {
        robotState.automatique = 1;
        stateRobot = STATE_ATTENTE;
    }
}