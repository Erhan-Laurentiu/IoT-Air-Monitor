
#ifndef MAX44009_H_
#define MAX44009_H_

#include"msp430.h"
#include"I2C.h"


#define MAX44009_ADDRESS 0x4A

void MAX44009_INIT();
void MAX44009_readRegOneByte(uint8_t startAddress, uint8_t* buffer);
float MAX44009_LUX();
void MAX44009_READ(volatile float *lux);

#endif
