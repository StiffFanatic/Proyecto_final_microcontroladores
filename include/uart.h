#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

extern SemaphoreHandle_t uart_mutex;

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void imprimir(char *buffer);

#endif