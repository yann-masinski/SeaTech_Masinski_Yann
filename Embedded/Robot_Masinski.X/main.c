#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();
    InitTimer1(250); //valeur en Hz des timers ici 250 Hz
    InitTimer23();
    InitTimer4(1000);
    InitPWM();
    InitADC1();

    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();

            unsigned int * result = ADCGetResult();
            float volts = ((float) result[1])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result[2])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result[4])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result[3])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremGauche = 34 / volts - 5;
            volts = ((float) result[0])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremDroit = 34 / volts - 5;


            if (robotState.distanceTelemetreDroit < 30 || robotState.distanceTelemetreExtremDroit < 30) LED_ORANGE = 1;
            else LED_ORANGE = 0;

            if (robotState.distanceTelemetreCentre < 30) LED_BLEUE = 1;
            else LED_BLEUE = 0;

            if (robotState.distanceTelemetreGauche < 30 || robotState.distanceTelemetreExtremGauche < 30) LED_BLANCHE = 1;
            else LED_BLANCHE = 0;


        }
    } // fin main
}

unsigned char stateRobot;
float vitesse = 25;
float vitessemanoeuvre = 25;
float disevitement = 33;
float demi = 0;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case DEMI_TOUR:
            PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_GAUCHE);
            demi = 0;
                    stateRobot = DEMI_TOUR_EN_COURS;
            break;
        case DEMI_TOUR_EN_COURS:
            demi = demi + 1;
            if (demi >= 175) {
                SetNextRobotStateInAutomaticMode();
            }
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;
float alter = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //Détermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreDroit < disevitement &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreGauche > disevitement) //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreGauche < disevitement &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreDroit > disevitement) //Obstacle à gauche        
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (robotState.distanceTelemetreCentre < disevitement) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;

    else if (robotState.distanceTelemetreExtremDroit < 22 &&
            robotState.distanceTelemetreDroit > disevitement &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreGauche > disevitement) //Obstacle à ex gauche        
        positionObstacle = OBSTACLE_A_EXDROITE;
    else if (robotState.distanceTelemetreExtremGauche < 22 &&
            robotState.distanceTelemetreGauche > disevitement &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreDroit > disevitement) //Obstacle à ex gauche        
        positionObstacle = OBSTACLE_A_EXGAUCHE;

    else if (robotState.distanceTelemetreDroit > disevitement &&
            robotState.distanceTelemetreExtremDroit > disevitement &&
            robotState.distanceTelemetreExtremGauche > disevitement &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreGauche > disevitement)
        positionObstacle = PAS_D_OBSTACLE;
        /*
         else if (robotState.distanceTelemetreDroit < disevitement &&
                robotState.distanceTelemetreExtremDroit < disevitement &&
                robotState.distanceTelemetreExtremGauche < disevitement &&
                robotState.distanceTelemetreCentre < disevitement &&
                robotState.distanceTelemetreGauche < disevitement)
            positionObstacle = OBSTACLE_AUTOUR;
         
    else if (robotState.distanceTelemetreDroit > disevitement &&
            robotState.distanceTelemetreExtremDroit < disevitement - 5 &&
            robotState.distanceTelemetreExtremGauche < disevitement - 5 &&
            robotState.distanceTelemetreCentre > disevitement &&
            robotState.distanceTelemetreGauche > disevitement)
        positionObstacle = OBSTACLE_COTE;
         */





    //Détermination de l?état à venir du robot

    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;

    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;

    else if (positionObstacle == OBSTACLE_EN_FACE) {
        if (alter == 3) {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            alter = 0;
        } else {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            alter = alter + 1;
        }
    } else if (positionObstacle == OBSTACLE_A_EXDROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;

    else if (positionObstacle == OBSTACLE_A_EXGAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;

    else if (positionObstacle == OBSTACLE_COTE)
        nextStateRobot = DEMI_TOUR;
    /*
    else if (positionObstacle == OBSTACLE_COTE)
        nextStateRobot = STATE_AVANCE;
     */
    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}