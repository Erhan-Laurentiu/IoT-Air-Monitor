#ifndef UART0_H_
#define UART0_H_

/*
*
*UART0 pins
*
*TX= P3.3
*RX= P3.4
*
*/



void UART0_Init();
void UART0_SendChar(char data);
void UART0_SendString_KnownLenght(char *buff,char length);
void UART0_SendString_UnknownLenght(char *buff);
char UART0_ReadChar();
void UART0_SendInt(int data);
void UART0_SendFloat(float data);
void UART0_SendNewLine();



#endif
