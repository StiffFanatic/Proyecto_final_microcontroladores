#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ===== Implementación Simplificada de FreeRTOS ===== */

/* Variables globales del scheduler */
static TickType_t xTickCount = 0;
static volatile uint8_t scheduler_running = 0;

/* Tareas */
#define MAX_TASKS 5
typedef struct
{
    TaskFunction_t pxTaskCode;
    void *pvParameters;
    uint16_t uxStackDepth;
    UBaseType_t uxPriority;
    uint8_t active;
    TickType_t xLastWakeTime;
} TaskControlBlock_t;

static TaskControlBlock_t tasks[MAX_TASKS];
static uint8_t task_count = 0;

/* Colas de mensajes */
#define MAX_QUEUES 3
typedef struct
{
    uint8_t *buffer;
    uint16_t buffer_size;
    uint16_t item_size;
    uint16_t item_count;
    uint16_t write_index;
    uint16_t read_index;
} QueueDescriptor_t;

static QueueDescriptor_t queues[MAX_QUEUES];
static uint8_t queue_count = 0;

/* Semáforos */
#define MAX_SEMAPHORES 3
typedef struct
{
    volatile uint8_t count;
} SemaphoreDescriptor_t;

static SemaphoreDescriptor_t semaphores[MAX_SEMAPHORES];
static uint8_t semaphore_count = 0;

/* ===== TASK FUNCTIONS ===== */

BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode,
    const char *const pcName,
    UBaseType_t usStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t *pxCreatedTask)
{
    if (task_count >= MAX_TASKS)
    {
        return pdFAIL;
    }

    tasks[task_count].pxTaskCode = pxTaskCode;
    tasks[task_count].pvParameters = pvParameters;
    tasks[task_count].uxStackDepth = usStackDepth;
    tasks[task_count].uxPriority = uxPriority;
    tasks[task_count].active = 1;
    tasks[task_count].xLastWakeTime = 0;

    if (pxCreatedTask)
    {
        *pxCreatedTask = (TaskHandle_t)&tasks[task_count];
    }

    task_count++;
    return pdPASS;
}

void vTaskDelayImpl(TickType_t xTicksToDelay)
{
    /* Aquí simplemente retornamos. En la versión real,
       la tarea se suspendería hasta que pasen los ticks */
    (void)xTicksToDelay;
}

void vTaskDelayUntilImpl(TickType_t *pxPreviousWakeTime, TickType_t xTimeIncrement)
{
    if (pxPreviousWakeTime != NULL)
    {
        *pxPreviousWakeTime += xTimeIncrement;
    }
}

TickType_t xTaskGetTickCountImpl(void)
{
    return xTickCount;
}

void vTaskStartSchedulerImpl(void)
{
    scheduler_running = 1;

    /* Loop principal del scheduler */
    while (1)
    {
        xTickCount++;

        /* Ejecutar todas las tareas */
        for (uint8_t i = 0; i < task_count; i++)
        {
            if (tasks[i].active && tasks[i].pxTaskCode != NULL)
            {
                tasks[i].pxTaskCode(tasks[i].pvParameters);
            }
        }

        /* Pequeña pausa para que el scheduler no consuma CPU al 100% */
        for (volatile uint16_t j = 0; j < 100; j++)
        {
            asm("nop");
        }
    }
}

/* ===== QUEUE FUNCTIONS ===== */

QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize)
{
    if (queue_count >= MAX_QUEUES)
    {
        return NULL;
    }

    uint16_t total_size = uxQueueLength * uxItemSize;
    uint8_t *buffer = (uint8_t *)malloc(total_size);

    if (buffer == NULL)
    {
        return NULL;
    }

    queues[queue_count].buffer = buffer;
    queues[queue_count].buffer_size = total_size;
    queues[queue_count].item_size = uxItemSize;
    queues[queue_count].item_count = 0;
    queues[queue_count].write_index = 0;
    queues[queue_count].read_index = 0;

    queue_count++;
    return (QueueHandle_t)&queues[queue_count - 1];
}

BaseType_t xQueueSend(
    QueueHandle_t xQueue,
    const void *const pvItemToQueue,
    TickType_t xTicksToWait)
{
    QueueDescriptor_t *q = (QueueDescriptor_t *)xQueue;

    if (q == NULL)
        return pdFAIL;

    /* Si la cola está llena, descartar o esperar */
    if (q->item_count >= (q->buffer_size / q->item_size))
    {
        if (xTicksToWait == 0)
        {
            return pdFAIL;
        }
        /* En versión simple, esperamos pero no bloqueamos */
    }

    /* Copiar item a la cola */
    uint8_t *dest = q->buffer + (q->write_index * q->item_size);
    memcpy(dest, pvItemToQueue, q->item_size);

    q->write_index = (q->write_index + 1) % (q->buffer_size / q->item_size);
    q->item_count++;

    return pdPASS;
}

BaseType_t xQueueReceive(
    QueueHandle_t xQueue,
    void *const pvBuffer,
    TickType_t xTicksToWait)
{
    QueueDescriptor_t *q = (QueueDescriptor_t *)xQueue;

    if (q == NULL)
        return pdFAIL;

    if (q->item_count == 0)
    {
        return pdFAIL;
    }

    /* Copiar item desde la cola */
    uint8_t *src = q->buffer + (q->read_index * q->item_size);
    memcpy(pvBuffer, src, q->item_size);

    q->read_index = (q->read_index + 1) % (q->buffer_size / q->item_size);
    q->item_count--;

    return pdPASS;
}

/* ===== SEMAPHORE FUNCTIONS ===== */

SemaphoreHandle_t xSemaphoreCreateBinary(void)
{
    if (semaphore_count >= MAX_SEMAPHORES)
    {
        return NULL;
    }

    semaphores[semaphore_count].count = 0; /* Inicialmente sin señal */

    semaphore_count++;
    return (SemaphoreHandle_t)&semaphores[semaphore_count - 1];
}

BaseType_t xSemaphoreTake(
    SemaphoreHandle_t xSemaphore,
    TickType_t xTicksToWait)
{
    SemaphoreDescriptor_t *sem = (SemaphoreDescriptor_t *)xSemaphore;

    if (sem == NULL)
        return pdFAIL;

    if (sem->count > 0)
    {
        sem->count--;
        return pdTRUE;
    }

    return pdFAIL;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
    SemaphoreDescriptor_t *sem = (SemaphoreDescriptor_t *)xSemaphore;

    if (sem == NULL)
        return pdFAIL;

    if (sem->count < 255)
    {
        sem->count++;
    }

    return pdPASS;
}
