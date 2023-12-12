#ifndef TACHEADC_H
#define TACHEADC_H


void myClockSwiFxn(uintptr_t);
void TacheADC_CreateTask(void);
void TacheADC_taskFxn(UArg, UArg);
uint32_t Sampling(uint_least8_t Board_ADC_Number);
float uVToG_float(uint32_t dataSampled);


#endif
