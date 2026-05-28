#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"

typedef void (*TaskFunction_t)(void *);
typedef void* TaskHandle_t;

#define configMAX_TASK_NAME_LEN 16

#define vTaskDelay(ms) vTaskDelayImpl(ms)
#define vTaskDelayUntil(pxPreviousWakeTime, xTimeIncrement) vTaskDelayUntilImpl(pxPreviousWakeTime, xTimeIncrement)
#define xTaskGetTickCount() xTaskGetTickCountImpl()
#define vTaskStartScheduler() vTaskStartSchedulerImpl()

BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode,
    const char * const pcName,
    UBaseType_t usStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t *pxCreatedTask
);

void vTaskDelayImpl(TickType_t xTicksToDelay);
void vTaskDelayUntilImpl(TickType_t *pxPreviousWakeTime, TickType_t xTimeIncrement);
TickType_t xTaskGetTickCountImpl(void);
void vTaskStartSchedulerImpl(void);

#define pdMS_TO_TICKS(ms) ((ms) / portTICK_PERIOD_MS)
#define portTICK_PERIOD_MS 1

#endif /* TASK_H */
