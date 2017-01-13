#ifndef _TRASNFER_TASK_H_
#define _TRASNFER_TASK_H_

#include "freertos_fit.h"

extern TaskHandle_t x_transfer_task;

extern xSemaphoreHandle xSemaphore_transfer;

void transfer_task(void *pvParameters);

#endif

