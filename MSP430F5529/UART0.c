#include <msp430.h>
#include <stdio.h>
#include "UART0.h"

char _bufff[20]={0};
void UART0_Init()
{
    P3SEL |= BIT3+BIT4;                         // UART1   3.3=TX 3.4=RX
    UCA0CTL1 |= UCSWRST;                        // UART reset enable (configuration mode)
    UCA0CTL1 |= UCSSEL_2;                       // SMCLK = 1MHZ
    UCA0BR0 = 9;                                // 1MHz/9 ~= 115200;
    UCA0BR1 = 0;                                // baudrate = freq/(br0+256*br1);
    UCA0MCTL |= UCBRS_1 + UCBRF_0;              // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                       // UART reset disable (exit configuration mode
    UCA0IE |= UCRXIE;                           // Enable USCI_A0 RX interrupt
    __bis_SR_register(GIE);                     // Enter LPM0, interrupts enabled

}


void UART0_SendChar(char data)
{
    while (!(UCA0IFG&UCTXIFG));                     //USCI_A0 TX bufffer ready?
    UCA0TXBUF = data;
}


void UART0_SendString_KnownLenght(char *bufff,char length)
{
    int i;
    for(i=0;i<length;i++,bufff++)
    {
        while (!(UCA0IFG&UCTXIFG));                 //USCI_A0 TX bufffer ready?
        UCA0TXBUF = *bufff;
    }
}


void UART0_SendString_UnknownLenght(char *buff)
{

    while(*buff != '\0'){
        while (!(UCA0IFG&UCTXIFG));                 // USCI_A0 TX buffer ready?
        UCA0TXBUF = *buff;
        buff++;
    }
}


char UART0_ReadChar()
{
    while(!(UCA0IFG&UCRXIFG));
    return UCA0RXBUF;
}


void UART0_SendInt(int data)
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
            UART0_SendChar(string[i]+'0');
        }


    }
}


//void UART0_SendFloat(float data)
//{
//    int n;
//    n=sprintf (_bufff, "%.2f\n", data);
//    UART0_SendString_KnownLenght(_bufff,n);
//}

void UART0_SendNewLine()
{

    UART0_SendChar('\r');
    UART0_SendChar('\n');
}



//#pragma vector=USCI_A0_VECTOR
//__interrupt void  USCI_A0_ISR(void)
//{
//    switch(__even_in_range(UCA0IV,4))
//    {
//    case 0:break;                               // Vector 0 - no interrupt
//    case 2:                                     // Vector 2 - RXIFG
//        while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX bufffer ready?
//        UCA0TXBUF = UCA0RXBUF;                  // TX -> RXed character (Echo)
//        break;
//    case 4: break;                              //Vector 4 - TXIFG
//    default: break;
//    }
//}
