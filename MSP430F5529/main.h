
#include <msp430.h>
#include"BME280.h"
#include"CCS811.h"
#include"MAX44009.h"
#include"I2C.h"
#include"UART1.h"
#include"UART0.h"
#include"Delay.h"

#ifndef MAIN_H_
#define MAIN_H_




inline void WATCHDOG_DISABLE();
void HW_INIT();
void SENSORS_INIT();
void SENSOR_READ(unsigned char counter);
void SendInt(long sensorValue);
void SENSORS_SEND_UART();
void SENSORS_READ();

void ESP_RESET();
void ESP_PIN_INIT();

#endif
