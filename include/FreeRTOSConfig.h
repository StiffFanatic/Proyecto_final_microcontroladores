/*
 * FreeRTOS Kernel V11.1.0
 * Configuration for ATmega328P
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <avr/io.h>

/* Básico */
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configTICK_RATE_HZ 1000
#define configMAX_PRIORITIES 4
#define configMINIMAL_STACK_SIZE 85

/* Memory */
#define configTOTAL_HEAP_SIZE 1024

/* Queue */
#define configQUEUE_REGISTRY_SIZE 0
#define configUSE_QUEUE_SETS 0

/* Task names */
#define configUSE_TRACE_FACILITY 0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Semáforos */
#define configUSE_MUTEXES 1
#define configUSE_RECURSIVE_MUTEXES 0
#define configUSE_COUNTING_SEMAPHORES 0

/* Runtime stats */
#define configGENERATE_RUN_TIME_STATS 0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Optional */
#define configUSE_ALTERNATIVE_API 0
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configUSE_MALLOC_FAILED_HOOK 0

/* Co-routines */
#define configUSE_CO_ROUTINES 0
#define configMAX_CO_ROUTINE_PRIORITIES 2

#endif /* FREERTOS_CONFIG_H */
