#include <msp430.h>

#define SS_HIGH P1OUT|=BIT0
#define SS_LOW P1OUT&=~BIT0
#define SS_RESET_COMMUNICATION SS_HIGH;\
        SS_LOW;

#define SS_START_COMMUNICATION SS_LOW
#define SS_STOP_COMMUNICATION SS_HIGH

#define SCLK    BIT5            // SPI Clock at P1.5
#define SOMI    BIT6            // SPI SOMI (Slave Out, Master In) at P1.6
#define SIMO    BIT7            // SPI SIMO (Slave In, Master Out) at P1.7
#define CS      BIT0            // CS (Chip Select) at P1.3




#define DUMMY_VALUE 0x00
void HW_SPI_init(void);

void SPI_write(unsigned char tx_data);

unsigned char SPI_read(void);
unsigned char SPI_transfer(unsigned char tx_data,unsigned char tx_data2);
void SPI_Transfer_Receive_MultipleBytes(unsigned char *TXdata,unsigned char lenght,unsigned char *RXdata);
unsigned char SPI_Transfer_MultipleBytes_Receive_One(unsigned char *TXdata,unsigned char lenght);
