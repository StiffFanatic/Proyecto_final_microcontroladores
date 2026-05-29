#include "pwm.h"

void PWM_Timer1_init(void) {

    DDRB |= (1 << MOTOR_IZQ) | (1 << MOTOR_DER);

    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
    TCCR1B |= (1 << CS11) | (1 << CS10); //Modo fast PWM, prescaler 64

    // Inicializar los motores apagados
    OCR1A = 0;
    OCR1B = 0;
}

void pwm_motor(uint8_t pin_motor, uint8_t pwm) {
    if (pin_motor == MOTOR_IZQ) {
        OCR1A = pwm;
    } 
    else if (pin_motor == MOTOR_DER) {
        OCR1B = pwm;
    }
}