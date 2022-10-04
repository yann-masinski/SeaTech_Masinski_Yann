/* 
 * File:   adc.h
 * Author: Table2
 *
 * Created on 4 octobre 2022, 13:48
 */

#ifndef ADC_H
#define	ADC_H

void InitADC1(void);

void ADC1StartConversionSequence();
unsigned int * ADCGetResult(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);

#endif	/* ADC_H */

