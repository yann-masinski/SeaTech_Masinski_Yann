/*

 * File:   IO.c

 */



#include <xc.h>

#include "IO.h"

void InitIO() {

    // IMPORTANT : d�sactiver les entr�es analogiques, sinon on perd les entr�es num�riques

    ANSELA = 0; // 0 desactive

    ANSELB = 0;

    ANSELC = 0;

    ANSELD = 0;

    ANSELE = 0;

    ANSELF = 0;

    ANSELG = 0;



    //********** Configuration des sorties : _TRISxx = 0 ********************************

    // LED

    _TRISC10 = 0; // LED Orange

    _TRISG6 = 0; //LED Blanche

    _TRISG7 = 0; // LED Bleue

    // Moteurs 
    // PIN      G B14 B15    D C6 C7
    _TRISB14 = 0;
    _TRISB15 = 0;

    _TRISC6 = 0;
    _TRISC7 = 0;

    //********** Configuration des entr�es : _TRISxx = 1 ********************************   

    // Entr� des capteurs 
    _TRISC0 = 1;    //milieu droit    1
    _TRISC11 = 1;   //milieu    2
    _TRISG9 = 1;    //milieu gauche 3
    _TRISB1 =1;     // extrem gauche  0
    _TRISE15=1;     // extrem droit 4


}
