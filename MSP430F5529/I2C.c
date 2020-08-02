#include "I2C.h"

uint8_t *PTxData;                     // Pointer to TX data
uint8_t TXByteCtr;
uint8_t sending;
uint8_t *PRxData;                     // Pointer to RX data
uint8_t RXByteCtr;
uint8_t RxBuffer[24];
uint8_t reading;
uint8_t stop=0;

inline void I2C_CLEAR_PORT()
{
    P3DIR|=0x03;
    P3OUT|=0x03;
    P3OUT&=0x00;
    P3OUT|=0x03;
    P3DIR&=0x00;
}

inline void I2C_SET_ADDRESS( uint8_t address )
{
    UCB0I2CSA=address;
}

inline void I2C_ENABLE_TX_RX_INTERRUPTS()
{
    UCB0IE |= UCTXIE + UCRXIE;                         // Enable TX interrupt

}

inline void I2C_INIT()
{
    P1OUT &= ~0x01;                           // P1.0 = 0
    P1DIR |= 0x01;                            // P1.0 output

    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
    UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    I2C_ENABLE_TX_RX_INTERRUPTS();
}

void sendI2C( uint8_t* data, uint8_t length, uint8_t _stop )
{
    if(_stop==STOP)
        stop=1;
    else
        stop=0;
    P1OUT |= 0x00;                          // P1.0 = 1
    PTxData = data;                         // TX array start address
    TXByteCtr = length;                     // Load TX byte counter
    sending=1;

    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition

    __bis_SR_register(GIE);                 // Enter LPM0, enable interrupts
    while(sending);
    P1OUT |= 0x01;                        // P1.0 = 1
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
}

void I2C_WRITE_BYTES( uint8_t* data, uint8_t length, uint8_t _stop)
{
    if(_stop==STOP)
        stop=1;
    else
        stop=0;
    P1OUT |= 0x00;                          // P1.0 = 1
    PTxData = data;                         // TX array start address
    TXByteCtr = length;                     // Load TX byte counter
    sending=1;

    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition

    __bis_SR_register(GIE);                 // Enter LPM0, enable interrupts
    while(sending);
    P1OUT |= 0x01;                          // P1.0 = 1
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
}

uint8_t I2C_READ_BYTE()
{
    PRxData = (uint8_t *)RxBuffer;          // Start of RX buffer
    RXByteCtr = 1;                          // Load RX byte counter
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    while(RXByteCtr);
    return RxBuffer[0];
}
uint8_t WORD_LSB=0;

unsigned int I2C_READ_WORD()
{
    unsigned int temp;
    RxBuffer[0]=0;
    RxBuffer[1]=0;
    PRxData = (uint8_t *)RxBuffer;          // Start of RX buffer
    RXByteCtr = 2;                          // Load RX byte counter
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    while(RXByteCtr);
    temp=((unsigned int)RxBuffer[0])<<8;
    temp|=((unsigned int)RxBuffer[1]);
    return temp;
}

void I2C_READ_BYTES( uint8_t length, uint8_t* pointer )
{
    int i;
    PRxData = (uint8_t *)RxBuffer;          // Start of RX buffer
    RXByteCtr = length;                     // Load RX byte counter
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    while(RXByteCtr);
    for(i=0;i<length;i++,pointer++)
        *pointer=RxBuffer[i];
    __no_operation();
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCB0IV,12))
    {
    case  0: break;                             // Vector  0: No interrupts
    case  2: break;                             // Vector  2: ALIFG
    case  4:
    case  6: break;                             // Vector  6: STTIFG
    case  8: break;                             // Vector  8: STPIFG
    case 10:                                    // Vector 10: RXIFG
        RXByteCtr--;                            // Decrement RX byte counter
        if (RXByteCtr)
        {
            *PRxData++ = UCB0RXBUF;             // Move RX data to address PRxData
            if (RXByteCtr == 1)                 // Only one byte left?
                UCB0CTL1 |= UCTXSTP;            // Generate I2C stop condition
        }
        else
        {
            *PRxData = UCB0RXBUF;               // Move final RX data to PRxData
        }
    case 12:                                    // Vector 12: TXIFG
        if (TXByteCtr)                          // Check TX byte counter
        {
            UCB0TXBUF = *PTxData++;             // Load TX buffer
            TXByteCtr--;                        // Decrement TX byte counter
        }
        else
        {
            if(stop)
            UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
            if(RXByteCtr==0) UCB0IFG &= ~UCRXIFG;

            UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
            sending=0;
        }
        break;
    default: break;
    }
}
