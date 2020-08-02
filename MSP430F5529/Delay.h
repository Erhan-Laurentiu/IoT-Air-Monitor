
#include<msp430f5529.h>
#ifndef DELAY_H_
#define DELAY_H_

void TIMER_A2_START_SMCLK();
void TIMER_A2_START_ACLK();

void DELAY_TIMER_A2_SMCLK(unsigned int calculatedValue);
void DELAY_TIMER_A2_ACLK(unsigned int calculatedValue);

void TIMER_A2_STOP();

void DELAY_TIMER_A2_ACLK_LongerPeriod(unsigned int etalon_buff);

#endif
