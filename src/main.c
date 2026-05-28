#include <stdio.h>
#include <avr/io.h>
#include "millis.h"
#include "uart.h"
#include "angulo.h"
#include "acotar.h"
#include "pwm.h"
#include "adc.h"

#define BAUD 9600
#define F_OSC 16000000
#define MYUBRR ((F_OSC/(16UL*BAUD))-1)

#define MOTOR_IZQ PB1
#define MOTOR_DER PB2

/// Constantes PID
char buffer[10] = "Iniciar\n";
float kp = 1.0;
float ki = 0.8;
float kd = 0.3;
double presente = 0, pasado = 0, dt = 0;
double error = 0, error_anterior = 0; 
double set_point = 0, salida;
double integral = 0, derivada = 0;
uint8_t PWM_base = 80;
int8_t angulo = 0;

// Variable de control para el estado del sistema
uint8_t sistema_activo = 0; // 0 = Apagado, 1 = Activo


int main(void){

    DDRB |= (1<<PB5);
    PORTB &= ~(1<<PB5);

    /// iniciar los periféricos
    timer0_init();
    PWM_Timer1_init();
    USART_Init(MYUBRR);
    imprimir(buffer); /// depuracion
    adc_configuracion();
    sei();

    while(1){

        // --- Lectura
        if (UCSR0A & (1 << RXC0)) {
            char c = USART_Receive(); // Lee el carácter entrante
            
            if (c == '1') {
                sistema_activo = 1;
            } 
            else if (c == '0') {
                sistema_activo = 0;
                /// apagado forzado de motores
                pwm_motor(MOTOR_IZQ, 0);
                pwm_motor(MOTOR_DER, 0);
            }
        }

        presente = obtener_millis();
        dt = (presente - pasado) / 1000.0;

        if(dt >= 0.05){
            //PORTB ^= (1<<PB5);
            
            /// Solo ejecuta el control si el sistema está activo
            if (sistema_activo) {
                /// Lectura del ADC
                int valor = adc();
                angulo = obtener_angulo(valor);
                sprintf(buffer, "%i\n", angulo);
                imprimir(buffer); /// prueba

                error = set_point - angulo; /// error del PID

                derivada = (error - error_anterior) / dt;
                integral += (error + error_anterior) / 2 * dt;
                integral = acotar_integral(integral); /// para evitar desborde por crecimiento

                salida = (kp * error) + (ki * integral) + (kd * derivada);

                int pwm_izq = acotar_salida(PWM_base + salida, 0, 255);
                int pwm_der = acotar_salida(PWM_base - salida, 0, 255);

                pwm_motor(MOTOR_IZQ, (uint8_t)pwm_izq);
                pwm_motor(MOTOR_DER, (uint8_t)pwm_der);

                error_anterior = error;
            }

            pasado = presente;
        }
    }

    return 0;
}