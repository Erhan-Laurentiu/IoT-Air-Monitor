#ifndef BME280_H_
#define BME280_H_
#include<msp430.h>
#include<SPI.h>
#include<stdint.h>

#define BME280_S32_t            long signed int
#define BME280_U32_t            long unsigned int
#define BME280_S64_t            long long signed int

#define BME280_REG_HUM_LSB      0xFE
#define BME280_REG_HUM_MSB      0xFD

#define BME280_REG_TEMP_XLSB    0xFC
#define BME280_REG_TEMP_LSB     0xFB
#define BME280_REG_TEMP_MSB     0xFA

#define BME280_REG_PRESS_XLSB   0xF9
#define BME280_REG_PRESS_LSB    0xF8
#define BME280_REG_PRESS_MSB    0xF7

// if you want to write register MSB must be 0
// if you want to read register MSB must be 1

#define BME280_REG_CONFIG       0xF5&0x7F
#define BME280_REG_CTRL_MEAS    0xF4&0x7F
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_HUM     0xF2&0x7F

#define BME280_REG_RESET        0xE0&0x7F
#define BME280_REG_ID           0xD0
#define BME280_DUMMY_REGISTER   0x00


typedef struct
{
    //    uint32_t uPressure;
    //    uint32_t cPressureInt;
    //    uint32_t cPressureFrac;
    //    uint32_t uTemperature;
    //    uint32_t uHumidity;


    int32_t cTemperatureCInt;
    int32_t cTemperatureCFrac;

    uint32_t cHumidityInt;
    uint32_t cHumidityFrac;

}BME280_Data;


typedef struct
{
    uint16_t dig_T1;        //temperature
    int16_t dig_T2;
    int16_t dig_T3;

    //    uint16_t dig_P1;        //pressure
    //    int16_t dig_P2;
    //    int16_t dig_P3;
    //    int16_t dig_P4;
    //    int16_t dig_P5;
    //    int16_t dig_P6;
    //    int16_t dig_P7;
    //    int16_t dig_P8;
    //    int16_t dig_P9;

    uint8_t dig_H1;         //humidity
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;


}BME280_CalibrationData;

uint8_t BME280_Read_ID(); //get the BME280 sensor chip ID: 0x60
void BME280_Reset(); //reset BME280
void BME280_writeReg(unsigned char reg,unsigned char value);
void BME280_readRegArray(uint8_t reg, uint8_t* buffer, uint8_t len);
void BME280_readCalibrationValues(void);
void BME280_READ(volatile long signed int *temperature, volatile long signed int *humidity);
void BME280_init();
void BME280_sleep(void);
BME280_Data BME280_GET_DATA(void);
#endif /* BME280_H_ */
