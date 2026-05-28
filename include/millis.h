#ifndef MILLIS_H
#define MILLIS_H

#include <avr/io.h>
#include <avr/interrupt.h>

uint32_t contador;
void timer0_init(void);
uint32_t obtener_millis(void);

#endif