#ifndef UART1_H_
#define UART1_H_

/*
*
*UART1 is connected to the usb
*
*/



void UART1_Init();
void UART1_SendChar(char data);
void UART1_SendString_KnownLenght(char *buff,char length);
void UART1_SendString_UnknownLenght(char *buff);
char UART1_ReadChar();
void UART1_SendInt(int data);
void UART1_SendFloat(float data);
void UART1_SendNewLine();


#endif
