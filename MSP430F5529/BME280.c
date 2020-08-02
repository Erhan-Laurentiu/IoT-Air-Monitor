#include <stddef.h>
#include <stdint.h>
#include "Delay.h"
#include "BME280.h"
#include "I2C.h"

static uint8_t BME280_readReg(uint8_t reg);
void BME280_readRegArray(uint8_t startAddress, uint8_t* buffer, uint8_t len);
static void BME280_writeReg(uint8_t reg, uint8_t value);
static void BME280_readCalibrationValues(void);
static volatile BME280_CalibrationData mCalibrationData;

BME280_S32_t t_fine;		//used in the temperature, pressure, humidity correction functions


void BME280_init(void)
{
    t_fine = 0x00;				//global used in compensation equations.
    I2C_SET_ADDRESS(BME280_I2C_ADDRESS);

    DELAY_TIMER_A2_ACLK(667); //20ms
    BME280_reset();
    DELAY_TIMER_A2_ACLK(667); //20ms

    BME280_writeReg(BME280_REG_CTRL_HUM, 0x01);			//1x oversample - enabled.
    BME280_writeReg(BME280_REG_CTRL_MEAS, 0x54);		//enable and sleep
    BME280_writeReg(BME280_REG_CONFIG, 0x10);
    BME280_writeReg(BME280_REG_CTRL_MEAS, 0x57);
    BME280_readCalibrationValues();

}

void BME280_reset(void)
{
    BME280_writeReg(BME280_REG_RESET, 0xB6);
}

uint8_t BME280_readChipID(void)
{
    uint8_t id = BME280_readReg(BME280_REG_ID);
    return id;
}

void BME280_sleep(void)
{
    uint8_t value = BME280_readReg(BME280_REG_CTRL_MEAS);
    value &=~ 0x03;				//bits 0 and 1 low

    BME280_writeReg(BME280_REG_CTRL_MEAS, value);
}

void BME280_wakeup(void)
{
    uint8_t value = BME280_readReg(BME280_REG_CTRL_MEAS);
    value |= 0x03;				//bits 0 and 1 high

    BME280_writeReg(BME280_REG_CTRL_MEAS, value);
}

uint8_t BME280_readReg(uint8_t reg)
{
    uint8_t result = 0x00;
    //	i2c_readAddress(BME280_I2C_ADDRESS, reg, &result, 1);
    //    setI2CAddress(BME280_I2C_ADDRESS);
    I2C_WRITE_BYTES(&reg,1,NO_STOP);
    result = I2C_READ_BYTE();
    return result;
}


void BME280_writeReg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = {reg, value};
    I2C_WRITE_BYTES(tx, 2 ,STOP);
}

void BME280_readRegArray(uint8_t startAddress, uint8_t* buffer, uint8_t len)
{

    I2C_WRITE_BYTES(&startAddress, 1, NO_STOP);
    I2C_READ_BYTES(len,buffer);

}



void BME280_readRegOneByte(uint8_t startAddress, uint8_t* buffer)
{

    I2C_WRITE_BYTES(&startAddress, 1, NO_STOP);
    *buffer = I2C_READ_BYTE();
}


void BME280_readCalibrationValues(void)
{
    //read calibration registers
    uint8_t reg[24] = {0x00};
    uint8_t a1 = 0x00;
    uint8_t e1[7] = {0x00};

    BME280_readRegArray(0x88, reg, 24);			//read 24 bytes of cal data
    BME280_readRegOneByte(0xA1,&a1);          //read 1 byte of cal data
    BME280_readRegArray(0xE1, e1, 7);			//read 3 bytes of cal data

    mCalibrationData.dig_T1 = ((uint16_t)reg[0]) + (((uint16_t)reg[1]) * 256);
    mCalibrationData.dig_T2 = ((int16_t)reg[2]) + (((int16_t)reg[3]) * 256);
    mCalibrationData.dig_T3 = ((int16_t)reg[4]) + (((int16_t)reg[5]) * 256);

    mCalibrationData.dig_P1 = ((uint16_t)reg[6]) + (((uint16_t)reg[7]) * 256);
    mCalibrationData.dig_P2 = ((int16_t)reg[8]) + (((int16_t)reg[9]) * 256);
    mCalibrationData.dig_P3 = ((int16_t)reg[10]) + (((int16_t)reg[11]) * 256);
    mCalibrationData.dig_P4 = ((int16_t)reg[12]) + (((int16_t)reg[13]) * 256);
    mCalibrationData.dig_P5 = ((int16_t)reg[14]) + (((int16_t)reg[15]) * 256);
    mCalibrationData.dig_P6 = ((int16_t)reg[16]) + (((int16_t)reg[17]) * 256);
    mCalibrationData.dig_P7 = ((int16_t)reg[18]) + (((int16_t)reg[19]) * 256);
    mCalibrationData.dig_P8 = ((int16_t)reg[20]) + (((int16_t)reg[21]) * 256);
    mCalibrationData.dig_P9 = ((int16_t)reg[22]) + (((int16_t)reg[23]) * 256);

    mCalibrationData.dig_H1 = a1;
    mCalibrationData.dig_H2 = ((int16_t)e1[0]) + (((int16_t)e1[1]) * 256);
    mCalibrationData.dig_H3 = e1[2];

    mCalibrationData.dig_H4 = ((int16_t)(e1[4] & 0x0F)) + (((int16_t)e1[3]) * 16);
    mCalibrationData.dig_H5 = ((int16_t)((e1[4] & 0xF0) >> 4)) + (((int16_t)e1[5]) * 16);
    mCalibrationData.dig_H6 = e1[6];

}

BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
    BME280_S32_t var1, var2, T;

    var1 = ((((adc_T>>3) - ((BME280_S32_t)mCalibrationData.dig_T1 << 1))) * ((BME280_S32_t)mCalibrationData.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t)mCalibrationData.dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)mCalibrationData.dig_T1))) >> 12) * ((BME280_S32_t)mCalibrationData.dig_T3)) >> 14;

    t_fine = var1 + var2;
    T  = (t_fine * 5 + 128) >> 8;
    return T;
}

BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P)
{
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)t_fine) - 128000;

    var2 = var1 * var1 * (BME280_S64_t)mCalibrationData.dig_P6;
    var2 = var2 + ((var1*(BME280_S64_t)mCalibrationData.dig_P5)<<17);
    var2 = var2 + (((BME280_S64_t)mCalibrationData.dig_P4)<<35);
    var1 = ((var1 * var1 * (BME280_S64_t)mCalibrationData.dig_P3)>>8) + ((var1 * (BME280_S64_t)mCalibrationData.dig_P2)<<12);
    var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)mCalibrationData.dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }

    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BME280_S64_t)mCalibrationData.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BME280_S64_t)mCalibrationData.dig_P8) * p) >> 19;  p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)mCalibrationData.dig_P7)<<4);
    return (BME280_U32_t)p;
}


BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H)
{
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)mCalibrationData.dig_H4) << 20) - (((BME280_S32_t)mCalibrationData.dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)mCalibrationData.dig_H6)) >> 10) * (((v_x1_u32r *
            ((BME280_S32_t)mCalibrationData.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)mCalibrationData.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)mCalibrationData.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r>>12);

}


void BME280_READ(volatile float *temperature, volatile float *humidity, volatile float *pressure)
{
    I2C_SET_ADDRESS(BME280_I2C_ADDRESS);

    uint8_t data[8] = {0x00};
    uint32_t msb, lsb, xlsb = 0x00;
    uint32_t uPressure, uTemperature, uHumidity = 0x00;

    BME280_readRegArray(BME280_REG_PRESS_MSB, data, 8);

    //Pressure - 20bits
    msb = ((uint32_t)data[0]) & 0xFF;
    lsb = ((uint32_t)data[1]) & 0xFF;
    xlsb = ((uint32_t)(data[2] >> 4)) & 0x0F;
    uPressure = ((msb << 12) | (lsb << 4) | (xlsb));

    //temperature - 20bits
    msb = ((uint32_t)data[3]) & 0xFF;
    lsb = ((uint32_t)data[4]) & 0xFF;
    xlsb = ((uint32_t)(data[5] >> 4)) & 0x0F;
    uTemperature = ((msb << 12) | (lsb << 4) | (xlsb));

    //humidity - 16 bits
    msb = ((uint32_t)data[6]) & 0xFF;
    lsb = ((uint32_t)data[7]) & 0xFF;
    uHumidity = ((msb << 8) | (lsb));



    //compute the corrected values using the raw data
    //and calibration values as described in the datasheet

    //compute the compensated temp
    BME280_S32_t cTemperature = BME280_compensate_T_int32((BME280_S32_t)uTemperature);  //div 100 to get int, % 100 for frac
    BME280_U32_t cPressure = BME280_compensate_P_int64((BME280_S32_t)uPressure);        //divide 256 for integer
    BME280_U32_t cHumidity = BME280_compensate_H_int32((BME280_S32_t)uHumidity);        //divide 1024 for integer

    *temperature = ((float)cTemperature/100);
    *humidity = ((float)cHumidity/1024);
    *pressure = ((float)cPressure/256)/100; //value in hPA

}

