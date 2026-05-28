#include "pwm.h"

void PWM_Timer1_init(void) {
    // 1. Configurar pines PB1 (OC1A) y PB2 (OC1B) como salidas
    DDRB |= (1 << MOTOR_IZQ) | (1 << MOTOR_DER);

    // 2. Configurar Timer 1 en Modo Fast PWM de 8 bits (Modo 5)
    // WGM10 = 1, WGM11 = 0, WGM12 = 1, WGM13 = 0 (Ver datasheet de ATmega328P)
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    // 3. Configurar salidas no invertidas en canales A y B
    // Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

    // 4. Configurar el preescalador en 64
    // Con F_CPU = 16MHz y modo 8 bits (TOP = 255): 
    // Frecuencia PWM = 16MHz / (64 * 256) ≈ 976.56 Hz (Ideal para motores DC pequeños/medianos)
    TCCR1B |= (1 << CS11) | (1 << CS10);

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