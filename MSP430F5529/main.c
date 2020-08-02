#include"main.h"

volatile float temperature[5]={0x00};
volatile float humidity[5]={0x00};
volatile float pressure[5]={0x00};
volatile float lux[5]={0x00};
volatile unsigned int eCO2[5]={0x00};
volatile unsigned int tVOC[5]={0x00};


int main(void)
{
    WATCHDOG_DISABLE();
    HW_INIT();
    DELAY_TIMER_A2_ACLK(667); //20ms
    SENSORS_INIT();
    DELAY_TIMER_A2_ACLK_LongerPeriod(15); //30s  delay
    UART0_SendChar('z'); //send ESP to deepSleep

    while(1)
    {
        SENSORS_READ();
        SENSORS_SEND_UART();

    }
}


inline void WATCHDOG_DISABLE()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}

void HW_INIT()
{
    I2C_INIT();
    UART1_Init();
    UART0_Init();
    ESP_PIN_INIT();
}


void SENSORS_INIT()
{
    BME280_init();
    initCCS811();
    MAX44009_INIT();
}



void SENSOR_READ(unsigned char counter)
{
    BME280_READ(&temperature[counter], &humidity[counter], &pressure[counter]);
    CCS811_READ(&eCO2[counter] , &tVOC[counter] );
    MAX44009_READ(&lux[counter]);
}


void SendInt(long sensorValue)
{
    long var=sensorValue;
    int i;
    char buff_uart[10]={0x00};

    for(i=9;i>=0;i--)
    {
        buff_uart[i]=(var%10)+'0';
        var=var/10;
    }
    UART1_SendString_KnownLenght(buff_uart,10);
    UART0_SendString_KnownLenght(buff_uart,10);
}

void SENSORS_SEND_UART()
{
    unsigned int i=0;
    ESP_RESET();
    DELAY_TIMER_A2_ACLK_LongerPeriod(7); //14 sec delay
    for(i=0;i<5;i++) SendInt((long)(temperature[i]*100));
    for(i=0;i<5;i++) SendInt((long)(humidity[i]*100));
    for(i=0;i<5;i++) SendInt((long)((float)(pressure[i])*1000));
    for(i=0;i<5;i++) SendInt((long)((float)(lux[i])*100));
    for(i=0;i<5;i++) SendInt((long)(eCO2[i]));
    for(i=0;i<5;i++) SendInt((long)(tVOC[i]));
}

void SENSORS_READ()
{
    unsigned int i=0;
    for( i=0;i<5;i++)
    {
        DELAY_TIMER_A2_ACLK_LongerPeriod(31); //1 min delay
        SENSOR_READ(i);
    }
}

void ESP_RESET()
{
    P2DIR|=BIT0; //OUTPUT HIGH
    P2REN|=BIT0;
    P2OUT|=BIT0;

    DELAY_TIMER_A2_ACLK_LongerPeriod(31);
    P2OUT&=~BIT0; //OUTPUT LOW
    DELAY_TIMER_A2_ACLK_LongerPeriod(31);
    P2OUT|=BIT0;
    P2DIR&=~BIT0; // INPUT PULLED UP

}

void ESP_PIN_INIT()
{
    P2DIR|=BIT0; // INPUT PULLED UP
    P2REN|=BIT0;
    P2OUT|=BIT0;

}




