#include"MAX44009.h"

#define MAX44009_ADDRESS 0x4A

void MAX44009_INIT()
{
    I2C_SET_ADDRESS(MAX44009_ADDRESS);
    unsigned char a[]={0x01,0x00,0x00};
    I2C_WRITE_BYTES(a,3,STOP);

}



void MAX44009_readRegOneByte(uint8_t startAddress, uint8_t* buffer)
{

    I2C_WRITE_BYTES(&startAddress, 1, NO_STOP);
    *buffer = I2C_READ_BYTE();
}

float MAX44009_LUX()
{
    unsigned char data[2]={0x00};
    MAX44009_readRegOneByte(0x03,&data[0]);          //read 1 byte of cal data
    MAX44009_readRegOneByte(0x04,&data[1]);          //read 1 byte of cal data
    int exponent = (data[0] & 0xF0) >> 4;
    int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);

    volatile float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
    //    volatile float luminance = pow(2, exponent) * mantissa * 0.045;
    return luminance;

}

void MAX44009_READ(volatile float *lux)
{
    I2C_SET_ADDRESS(MAX44009_ADDRESS);
    unsigned char data[2]={0x00};
    MAX44009_readRegOneByte(0x03,&data[0]);          //read 1 byte of cal data
    MAX44009_readRegOneByte(0x04,&data[1]);          //read 1 byte of cal data

    int exponent = (data[0] & 0xF0) >> 4;
    int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);

    *lux = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045) ;

}
