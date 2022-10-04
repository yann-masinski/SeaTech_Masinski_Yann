#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"

unsigned int ADCValue0, ADCValue1, ADCValue2;

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();
    InitTimer1();
    InitTimer23();
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
            
            unsigned int * result = ADCGetResult();
            ADCValue0 = result[0];
            if (ADCValue0>310) LED_ORANGE = 1;
            else LED_ORANGE=0;
            
            ADCValue1 = result[1];
            if (ADCValue1>310) LED_BLEUE = 1;
            else LED_BLEUE=0;
            
            ADCValue2 = result[2];
            if (ADCValue2>310) LED_BLANCHE = 1;
            else LED_BLANCHE=0;
            
            ADCClearConversionFinishedFlag();
        }
    } // fin main
}