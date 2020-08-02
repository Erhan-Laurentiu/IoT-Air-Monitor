#ifndef I2C_H
#define I2C_H

#include <msp430.h>
#include <stddef.h>
#include <stdint.h>

#define STOP  1
#define NO_STOP 0
void I2C_INIT();
void I2C_CLEAR_PORT();
inline void I2C_ENABLE_TX_RX_INTERRUPTS();
void I2C_SET_ADDRESS( uint8_t address );
void sendI2C( uint8_t* data, uint8_t length, uint8_t _stop );
void I2C_WRITE_BYTES( uint8_t* data, uint8_t length, uint8_t _stop );
uint8_t I2C_READ_BYTE();
unsigned int I2C_READ_WORD();
void I2C_READ_BYTES( uint8_t length, uint8_t* pointer );

#endif

