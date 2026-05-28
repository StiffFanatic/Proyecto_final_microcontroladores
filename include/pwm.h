#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

// Definición de pines para claridad (coinciden con tus defines)
#define MOTOR_IZQ PB1 // OC1A
#define MOTOR_DER PB2 // OC1B

/**
 * @brief Inicializa el Timer 1 en modo Fast PWM de 8 bits (Modo 5).
 * Configura los pines PB1 y PB2 como salidas y limpia los canales de PWM.
 */
void PWM_Timer1_init(void);

/**
 * @brief Controla el ciclo de trabajo (duty cycle) del motor seleccionado.
 * @param pin_motor Pin del motor a controlar (MOTOR_IZQ o MOTOR_DER).
 * @param pwm Valor del ciclo de trabajo entre 0 y 255.
 */
void pwm_motor(uint8_t pin_motor, uint8_t pwm);

#endif /* PWM_H_ */