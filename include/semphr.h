#ifndef SEMPHR_H
#define SEMPHR_H

#include "FreeRTOS.h"

typedef void* SemaphoreHandle_t;

SemaphoreHandle_t xSemaphoreCreateBinary(void);

BaseType_t xSemaphoreTake(
    SemaphoreHandle_t xSemaphore,
    TickType_t xTicksToWait
);

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);

#endif /* SEMPHR_H */
