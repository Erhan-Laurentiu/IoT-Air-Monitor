#include"SPI.h"



void HW_SPI_init(void)

{

    UCB0CTL1 = UCSWRST;
    P1DIR|=BIT0;
    P1SEL  |= SOMI + SIMO + SCLK;
    P1SEL2 |= SOMI + SIMO + SCLK;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;
    UCB0CTL1 |= UCSSEL_2;   // SMCLK
    UCB0CTL1 &= ~UCSWRST;
    SS_STOP_COMMUNICATION;
}


void SPI_Transfer_Receive_MultipleBytes(unsigned char *TXdata,unsigned char lenght,unsigned char *RXdata)
{
    int i=0;

    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = (*TXdata);
    TXdata++;
    //don't read first byte because it's junk
    while(UCB0STAT & UCBUSY);
    while(!(IFG2 & UCB0RXIFG));
    i = UCB0RXBUF;
    for(i=0;i<lenght;i++){
        while(!(IFG2 & UCB0TXIFG));
        UCB0TXBUF = (*TXdata);
        TXdata++;
        while(UCB0STAT & UCBUSY);
        while(!(IFG2 & UCB0RXIFG));
        *RXdata = UCB0RXBUF;
        RXdata++;
    }
}

unsigned char SPI_Transfer_MultipleBytes_Receive_One(unsigned char *TXdata,unsigned char lenght)
{
    int i=0;
    for(i=0;i<lenght;i++){
        while(!(IFG2 & UCB0TXIFG));
        UCB0TXBUF = (*TXdata);
        TXdata++;
    }
    while(UCB0STAT & UCBUSY);
    while(!(IFG2 & UCB0RXIFG));
    return UCB0RXBUF;
}

