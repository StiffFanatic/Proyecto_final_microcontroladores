#include "adc.h"

volatile uint16_t valor_adc = 0;

void adc_configuracion(void){
/* Canal ADC1, referencia AVCC, prescaler /128, con ISR */
 ADMUX = (1 << REFS0) |  0x0; /* Canal ADC1 (MUX[3:0]=0001) *///
 ADCSRA = (1 << ADEN)
 | (1 << ADIE) /* Habilitar ISR del ADC */
 | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

 ADCSRA |= (1 << ADSC); /* Primera conversion */

}
int adc(void){
valor_adc = ADC; /* Leer resultado de 10 bits */

//lcd_print(buffer);
 ADCSRA |= (1 << ADSC); /* Relanzar conversion */
return valor_adc;
}


 ISR(ADC_vect){
 adc();
 }