#include "adc.h"

void adc_configuracion(void){
    /* Canal ADC1, referencia AVCC, prescaler /128 */
    ADMUX = (1 << REFS0) | 0x0; 
    ADCSRA = (1 << ADEN)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int adc(void){
    /* Iniciar conversión ADC y esperar a que termine */
    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADIF))) {
        ;
    }
    ADCSRA |= (1 << ADIF);
    return ADC;
}
