
#include "uart.h"

SemaphoreHandle_t uart_mutex = NULL;

void USART_Init(unsigned int ubrr)
{
    
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void USART_Transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

unsigned char USART_Receive(void)
{
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void imprimir(char *buffer)
{
    int i = 0;
    // Si hay mutex, proteger la transmisión para evitar mezcla de bytes
    if (uart_mutex != NULL) {
        if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            while (buffer[i] != '\0')
            {
                USART_Transmit(buffer[i]);
                i++;
            }
            xSemaphoreGive(uart_mutex);
        }
    } else {
        // Si no hay mutex, hacer transmisión sin protección
        while (buffer[i] != '\0')
        {
            USART_Transmit(buffer[i]);
            i++;
        }
    }
}
