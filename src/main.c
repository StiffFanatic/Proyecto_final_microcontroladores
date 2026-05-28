#include <stdio.h>
#include <avr/io.h>
#include "millis.h"
#include "uart.h"
#include "angulo.h"
#include "acotar.h"
#include "pwm.h"
#include "adc.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD 9600
#define F_OSC 16000000
#define MYUBRR ((F_OSC/(16UL*BAUD))-1)

#define MOTOR_IZQ PB1
#define MOTOR_DER PB2

/// Constantes PID
char buffer[20] = "Iniciar\n";
float kp = 1.0;
float ki = 0.8;
float kd = 0.3;
double error = 0, error_anterior = 0; 
double set_point = 0, salida;
double integral = 0, derivada = 0;
uint8_t PWM_base = 80;
int8_t angulo = 0;

/* FreeRTOS - Colas y Semáforos */
QueueHandle_t cola_adc = NULL;           // Cola para valores ADC
SemaphoreHandle_t sem_sistema = NULL;    // Semáforo para activar/desactivar sistema

/* ========== TAREA: Lectora de ADC ========== */
void task_adc_reader(void *pvParameters) {
    int valor_adc = 0;
    
    imprimir("TASK_ADC iniciada\n");
    
    for (;;) {
        // Leer el ADC
        valor_adc = adc();
        
        // Enviar a la cola de PID
        if (xQueueSend(cola_adc, &valor_adc, portMAX_DELAY) != pdPASS) {
            imprimir("ERROR: Cola ADC llena\n");
        }
        
        // Esperar 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ========== TAREA: Control PID ========== */
void task_control_pid(void *pvParameters) {
    int valor_adc = 0;
    TickType_t xLastWakeTime;
    
    imprimir("TASK_PID iniciada\n");
    xLastWakeTime = xTaskGetTickCount();
    
    for (;;) {
        // Esperar exactamente 50 ms
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
        
        // Recibir el último valor ADC
        if (xQueueReceive(cola_adc, &valor_adc, 0) == pdPASS) {
            angulo = obtener_angulo(valor_adc);
            
            // Calcular error
            error = set_point - angulo;
            
            // Calcular PID
            const double dt = 0.05;  // 50 ms
            derivada = (error - error_anterior) / dt;
            integral += (error + error_anterior) / 2 * dt;
            integral = acotar_integral(integral);
            
            salida = (kp * error) + (ki * integral) + (kd * derivada);
            
            // Calcular PWM
            int pwm_izq = acotar_salida(PWM_base + salida, 0, 255);
            int pwm_der = acotar_salida(PWM_base - salida, 0, 255);
            
            // Aplicar control solo si el semáforo está en alto (sistema activo)
            if (xSemaphoreTake(sem_sistema, 0) == pdTRUE) {
                xSemaphoreGive(sem_sistema);  // Devolver inmediatamente
                pwm_motor(MOTOR_IZQ, (uint8_t)pwm_izq);
                pwm_motor(MOTOR_DER, (uint8_t)pwm_der);
                
                // Debug: enviar ángulo
                sprintf(buffer, "Ang:%i\n", angulo);
                imprimir(buffer);
            } else {
                // Sistema desactivado
                pwm_motor(MOTOR_IZQ, 0);
                pwm_motor(MOTOR_DER, 0);
            }
            
            error_anterior = error;
        }
    }
}

/* ========== TAREA: Comando UART ========== */
void task_uart_command(void *pvParameters) {
    char c;
    
    imprimir("TASK_UART iniciada\n");
    
    for (;;) {
        // Revisar si hay datos en UART (sin bloquear)
        if (UCSR0A & (1 << RXC0)) {
            c = USART_Receive();
            
            if (c == '1') {
                // Activar sistema
                xSemaphoreGive(sem_sistema);
                imprimir("SYS:ACTIVE\n");
            } 
            else if (c == '0') {
                // Desactivar sistema
                xSemaphoreTake(sem_sistema, 0);
                pwm_motor(MOTOR_IZQ, 0);
                pwm_motor(MOTOR_DER, 0);
                imprimir("SYS:INACTIVE\n");
            }
        }
        
        // Ejecutar cada 50 ms
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* ========== MAIN ========== */
int main(void) {
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    /// Iniciar periféricos
    timer0_init();
    PWM_Timer1_init();
    USART_Init(MYUBRR);
    adc_configuracion();
    
    imprimir("=== FreeRTOS INIT ===\n");

    /* Crear cola para ADC (10 valores almacenados) */
    cola_adc = xQueueCreate(10, sizeof(int));
    
    /* Crear semáforo binario (inicialmente vacío = sistema inactivo) */
    sem_sistema = xSemaphoreCreateBinary();

    if (cola_adc == NULL || sem_sistema == NULL) {
        imprimir("ERROR: Queue/Semaphore\n");
        return 1;
    }

    /* Crear tareas con prioridades */
    xTaskCreate(task_adc_reader,   "ADC_Reader",   200, NULL, 2, NULL);
    xTaskCreate(task_control_pid,  "PID_Control",  300, NULL, 3, NULL);
    xTaskCreate(task_uart_command, "UART_Cmd",     200, NULL, 1, NULL);

    imprimir("Tasks OK. Scheduler Start\n");
    
    /* Habilitar interrupciones y iniciar scheduler */
    sei();
    vTaskStartScheduler();

    // Nunca debe llegar aquí
    imprimir("ERROR: Scheduler END\n");
    return 0;
}