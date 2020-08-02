#include"CCS811.h"

#define CCS811_ADDRESS 0x5A

void resetCCS811()
{
    I2C_SET_ADDRESS(CCS811_ADDRESS);
    unsigned char resetData[5]= {0xFF,0x11,0xE5,0x72,0x8A}; //0xFF register  , 0x11,0xE5.. data;
    I2C_WRITE_BYTES(resetData,4,STOP);

}

void readRegOneByte(uint8_t startAddress, uint8_t* buffer)
{
    I2C_WRITE_BYTES(&startAddress, 1, NO_STOP);
    *buffer = I2C_READ_BYTE();
}


unsigned char checkID()
{
    unsigned char ID=0;
    readRegOneByte(0x20,&ID);          //read 1 byte of calibration data
    if(ID==0x81) return 0;             //everything is ok
    else return 1;                     // something went wrong


}

void writeApplicationStart()
{
    unsigned char ApplicationStart=0xF4;
    I2C_WRITE_BYTES(&ApplicationStart, 1, STOP);
}


unsigned char checkStatus()
{
    unsigned char status=0;
    readRegOneByte(0x00,&status);          //read 1 byte of calibration data
    return status;



}

void read1Min()
{
    unsigned char data[]={0x01,0x30}; //00110000
    I2C_WRITE_BYTES(data, 2, STOP);

}


void initCCS811()
{
    P2DIR|=BIT2;
    P2OUT&=~BIT2;                       //LOW ->MODULE IS ACTIVE

    I2C_SET_ADDRESS(CCS811_ADDRESS);    //set ccs811 address
    resetCCS811();
    checkID();
    writeApplicationStart();
    checkStatus();
    read1Min();                         //LPM
    P2OUT|=BIT2;                        //HIGH ->MODULE IS IN SLEEP

}


void readRegArray(uint8_t startAddress, uint8_t* buffer, uint8_t len)
{

    I2C_WRITE_BYTES(&startAddress, 1, NO_STOP);
    I2C_READ_BYTES(len,buffer);

}


void readData()
{
    P2OUT&=~BIT2; //LOW ->MODULE IS ACTIVE

    volatile unsigned int eco2=0;
    volatile unsigned int tvoc=0;
    unsigned char buffer[8]={0x00};
    readRegArray(0x02,buffer,8);
    P2OUT|=BIT2; //HIGH ->MODULE IS IN SLEEP

    //get data
    eco2=buffer[0]*256+buffer[1];
    tvoc = buffer[2]*256+buffer[3];
    tvoc = buffer[2]*256+buffer[3];



}

void CCS811_READ(volatile unsigned int *eCO2 , volatile unsigned int *tVOC )
{
    P2OUT&=~BIT2; //LOW ->MODULE IS ACTIVE

    I2C_SET_ADDRESS(CCS811_ADDRESS);

    volatile unsigned int eco2=0;
    volatile unsigned int tvoc=0;
    unsigned char buffer[8]={0x00};
    readRegArray(0x02,buffer,8);
    P2OUT|=BIT2; //HIGH ->MODULE IS IN SLEEP
    *eCO2 = buffer[0]*256 + buffer[1];
    *tVOC = buffer[2]*256 + buffer[3];

}
