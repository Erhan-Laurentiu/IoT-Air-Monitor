#include"I2C.h"
#ifndef CCS811_H_
#define CCS811_H_


void initCCS811();
void readData();
void CCS811_READ(volatile unsigned int *eCO2 , volatile unsigned int *tVOC );
#endif
