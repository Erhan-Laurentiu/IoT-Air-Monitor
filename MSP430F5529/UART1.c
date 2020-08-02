#include <msp430.h>
#include <stdio.h>
#include "UART1.h"

char _buff[20]={0};
void UART1_Init()
{
    P4SEL |= BIT4+BIT5;                         // UART1   4.4=TX 4.5=RX
    UCA1CTL1 |= UCSWRST;                        // UART reset enable (configuration mode)
    UCA1CTL1 |= UCSSEL_2;                       // SMCLK = 1MHZ
    UCA1BR0 = 9;                                // 1MHz/9 ~= 115200;
    UCA1BR1 = 0;                                // baudrate = freq/(br0+256*br1);
    UCA1MCTL |= UCBRS_1 + UCBRF_0;              // Modulation UCBRSx=1, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                       // UART reset disable (exit configuration mode
    UCA1IE |= UCRXIE;                           // Enable USCI_A1 RX interrupt
    __bis_SR_register(GIE);                     // Enter LPM0, interrupts enabled

}


void UART1_SendChar(char data)
{
    while (!(UCA1IFG&UCTXIFG));                     //USCI_A1 TX buffer ready?
    UCA1TXBUF = data;
}


void UART1_SendString_KnownLenght(char *buff,char length)
{
    int i;
    for(i=0;i<length;i++,buff++)
    {
        while (!(UCA1IFG&UCTXIFG));                 //USCI_A1 TX buffer ready?
        UCA1TXBUF = *buff;
    }
}


void UART1_SendString_UnknownLenght(char *buff)
{

    while(*buff != '\0'){
        while (!(UCA1IFG&UCTXIFG));                 // USCI_A0 TX buffer ready?
        UCA1TXBUF = *buff;
        buff++;
    }
}


char UART1_ReadChar()
{
    while(!(UCA1IFG&UCRXIFG));
    return UCA1RXBUF;
}


void UART1_SendInt(int data)
{
    if(data>=0){
        int i=0;
        unsigned int lastdigit ;
        char string[6]={0}; // assign 6 bytes for max 6 digits

        while(data!=0)
        {
            lastdigit=data%10;
            data=data/10;
            string[i]=lastdigit;
            i++;
        }
        i--;
        for(;i>=0;i--)
        {
            UART1_SendChar(string[i]+'0');
        }


    }
}


//void UART1_SendFloat(float data)
//{
//    int n;
//    n=sprintf (_buff, "%.2f\n", data);
//    UART1_SendString_KnownLenght(_buff,n);
//}

void UART1_SendNewLine()
{

    UART1_SendChar('\r');
    UART1_SendChar('\n');
}



#pragma vector=USCI_A1_VECTOR
__interrupt void  USCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV,4))
    {
    case 0:break;                               // Vector 0 - no interrupt
    case 2:                                     // Vector 2 - RXIFG
        while (!(UCA1IFG&UCTXIFG));             // USCI_A1 TX buffer ready?
        UCA1TXBUF = UCA1RXBUF;                  // TX -> RXed character (Echo)
        break;
    case 4: break;                              //Vector 4 - TXIFG
    default: break;
    }
}
