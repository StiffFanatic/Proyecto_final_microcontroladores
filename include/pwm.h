#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

#define MOTOR_IZQ PB1 // OC1A
#define MOTOR_DER PB2 // OC1B

void PWM_Timer1_init(void);
void pwm_motor(uint8_t pin_motor, uint8_t pwm);

#endif