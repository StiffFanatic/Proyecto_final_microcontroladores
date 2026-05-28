#ifndef UART_H
#define UART_H

#include<avr/io.h>

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void imprimir(char *buffer);

#endif