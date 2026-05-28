#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

 /* Compartida con la ISR */
void adc_configuracion(void);
int adc(void);
#endif