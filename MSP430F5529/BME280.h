
#ifndef SENSORS_BME280_H_
#define SENSORS_BME280_H_

#include <stddef.h>
#include <stdint.h>

#define BME280_S32_t			long signed int
#define BME280_U32_t			long unsigned int
#define BME280_S64_t			long long signed int

//#define BME280_I2C_ADDRESS		((uint8_t)(0x76 << 1))		//upshifted
#define BME280_I2C_ADDRESS		(uint8_t)(0x77)					//not upshifted

#define BME280_REG_HUM_LSB		0xFE
#define BME280_REG_HUM_MSB		0xFD

#define BME280_REG_TEMP_XLSB	0xFC
#define BME280_REG_TEMP_LSB		0xFB
#define BME280_REG_TEMP_MSB		0xFA

#define BME280_REG_PRESS_XLSB	0xF9
#define BME280_REG_PRESS_LSB	0xF8
#define BME280_REG_PRESS_MSB	0xF7

#define BME280_REG_CONFIG		0xF5
#define BME280_REG_CTRL_MEAS	0xF4
#define BME280_REG_STATUS		0xF3
#define BME280_REG_CTRL_HUM		0xF2

#define BME280_REG_RESET		0xE0
#define BME280_REG_ID			0xD0

typedef struct
{
	uint16_t dig_T1;		//temperature
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;		//pressure
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	uint8_t dig_H1;			//humidity
	int16_t dig_H2;
	uint8_t dig_H3;

	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;


}BME280_CalibrationData;

void BME280_READ(volatile float *temperature, volatile float *humidity, volatile float *pressure);



void BME280_init(void);
void BME280_reset(void);
uint8_t BME280_readChipID(void);

void BME280_sleep(void);
void BME280_wakeup(void);


BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P);
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H);

void BME280_readRegOneByte(uint8_t startAddress, uint8_t* buffer);

#endif
