#ifndef TIMER_H
#define TIMER_H


void InitTimer1(float freq);
void InitTimer23(void);
void InitTimer4(float freq);
void PWMUpdateSpeed(void);
void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);

extern unsigned long timestamp;
#endif 