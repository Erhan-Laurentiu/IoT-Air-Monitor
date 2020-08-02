#include"Delay.h"

int etalon=0xff;

void TIMER_A2_START_SMCLK()
{
    TA2CCTL0= CCIE;
    TA2CTL = TASSEL_2 + ID_3 + MC_1; // smclk/8 =~125KHz  up mode

}

void TIMER_A2_START_ACLK()
{

    TA2CCTL0= CCIE;
    TA2CTL = TASSEL_1  + MC_1 ; // ACLK =~32768  up mode

}

void DELAY_TIMER_A2_SMCLK(unsigned int calculatedValue )
{
    //calculatedValue = delayWanted/8    (delayWanted = [us])
    //max(calculatedValue) = 2^16 = 65536
    TA2CCR0 = calculatedValue-1;
    TIMER_A2_START_SMCLK();
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM3 w/ interrupt (CPU->OFF)
    TIMER_A2_STOP();


}
void DELAY_TIMER_A2_ACLK(unsigned int calculatedValue)
{
    //calculatedValue = delayWanted/30.517578125    (delayWanted = [us])
    //max(calculatedValue) = 2^16 = 65536
    TA2CCR0 = calculatedValue-1;
    TIMER_A2_START_ACLK();
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupt (CPU->OFF)
    TIMER_A2_STOP();

}

void TIMER_A2_STOP()
{
    TA2CTL = MC_0;
    TA2R=0;

}

void DELAY_TIMER_A2_ACLK_LongerPeriod(unsigned int calculatedValue)
{
    //delay= calculated value*2 secunde
    etalon= calculatedValue-1;
    TA2CCTL0&= ~CCIE;
    TA2CTL = TASSEL_1 + MC_2 + TACLR + TAIE;  // SMCLK, contmode, clear TAR
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupt (CPU->OFF)
    TIMER_A2_STOP();

}



// Timer2 A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer2_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A0_VECTOR))) Timer2_A0 (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM3_bits);   //Exit LPM3 (CPU->ON)

}



// Timer1_A3 Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A1_VECTOR))) TIMER2_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA2IV,14))
    {
    case  0: break;                          // No interrupt
    case  2: break;                          // CCR1 not used
    case  4: break;                          // CCR2 not used
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14: if(etalon==0) {                 // overflow
        __bic_SR_register_on_exit(LPM3_bits);   //Exit LPM3 (CPU->ON)

    }
    else etalon--;
    break;
    default: break;
    }
}
