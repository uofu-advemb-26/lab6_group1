/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "semphr.h"

#define SUPERVISOR_TASK_PRIORITY      ( tskIDLE_PRIORITY + 4UL )
#define TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define LOW_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MEDIUM_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2UL )
#define HIGH_TASK_PRIORITY      ( tskIDLE_PRIORITY + 3UL )

SemaphoreHandle_t mutex;

void low_task(__unused void *params)
{
    printf("Low priority task started running\n");
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
    {
        printf("Low priority task took mutex\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Low priority task giving mutex\n");
        if (xSemaphoreGive(mutex) == pdTRUE)
        {
            // This shouldn't be output because giving the mutex drops this task
            // back to low priority, where it will be starved by the medium
            // priority task
            printf("Low priority task gave mutex\n");
        }
        else
        {
            printf("Low priority task failed to give mutex\n");
        }
    }
    else
    {
        printf("Low priority task couldn't take mutex\n");
    }

    while (1)
    {
        vTaskDelay(1);
    }
}

void medium_task(__unused void *params)
{
    printf("Medium task started running\n");
    while (1); // Starvation
}

void high_task(__unused void *params)
{
    printf("High priority task started running\n");
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
    {
        printf("High priority task took mutex\n");
        vTaskDelay(pdMS_TO_TICKS(100));
        if (xSemaphoreGive(mutex) == pdTRUE)
        {
            printf("High priority task gave mutex\n");
        }
        else
        {
            printf("High priority task failed to give mutex\n");
        }
    }
    else
    {
        printf("High priority task couldn't take mutex\n");
    }

    while (1)
    {
        vTaskDelay(1);
    }
}

void supervisor_task(__unused void *params) {
    // Give time to attach to terminal
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Create the shared mutex
    if ((mutex = xSemaphoreCreateMutex()) == NULL)
    {
        printf("Unable to create mutex\n");
    }
    else
    {
        printf("Mutex created successfully\n");
    }

    // The low priority thread should have time to take the mutex before the
    // medium priority task starves the system or the high priority task tries to
    // take the mutex
    xTaskCreate(low_task, "LowThread",
                TASK_STACK_SIZE, NULL, LOW_TASK_PRIORITY, NULL);

    // Medium priority thread runs after low priority thread has taken mutex
    vTaskDelay(pdMS_TO_TICKS(100));
    xTaskCreate(medium_task, "MediumThread",
                TASK_STACK_SIZE, NULL, MEDIUM_TASK_PRIORITY, NULL);

    // By the time the high priority thread runs, the low priority thread has taken the mutex
    // but not yet given it
    vTaskDelay(pdMS_TO_TICKS(100));
    xTaskCreate(high_task, "HighThread",
                TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, NULL);   

    // Supervisor no longer needed; delete to prevent interference with other tasks
    vTaskDelete(NULL);
}

int main( void )
{
    stdio_init_all();
    TaskHandle_t task;
    xTaskCreate(supervisor_task, "SupervisorThread",
                TASK_STACK_SIZE, NULL, SUPERVISOR_TASK_PRIORITY, NULL);
    vTaskStartScheduler();
    return 0;
}
