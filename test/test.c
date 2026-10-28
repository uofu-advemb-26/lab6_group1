#include <stdio.h>
#include <pico/stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "busy_functions.h"

#define TASK_STACK_SIZE configMINIMAL_STACK_SIZE

void setUp(void) {}

void tearDown(void) {}

typedef void (*TaskFunc)(void);

void get_thread_stats(TaskFunction_t task1_func, TaskFunction_t task2_func, UBaseType_t task1_priority, UBaseType_t task2_priority, uint32_t *task1_time, uint32_t *task2_time)
{
    TaskHandle_t task1_handle, task2_handle;

    xTaskCreate(task1_func, "Task1",
                TASK_STACK_SIZE, NULL, task1_priority, &task1_handle);

    vTaskDelay(pdMS_TO_TICKS(100));

    xTaskCreate(task2_func, "Task2",
                TASK_STACK_SIZE, NULL, task2_priority, &task2_handle);

    vTaskDelay(pdMS_TO_TICKS(1000));

    *task1_time = ulTaskGetRunTimeCounter(task1_handle);
    *task2_time = ulTaskGetRunTimeCounter(task2_handle);

    vTaskDelete(task1_handle);
    vTaskDelete(task2_handle);     
}

void test_same_busybusy()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_busy, (TaskFunction_t)busy_busy, (tskIDLE_PRIORITY + 1UL), (tskIDLE_PRIORITY + 1UL), &time1, &time2);

    // 600,000 +- 20,000 microseconds
    // (The shorter time for task2 is to account for the 100us delay before starting task2 after starting task1)
    // Accounting for this, the time of the two tasks should be roughly equal.
    TEST_ASSERT_UINT32_WITHIN(20000, 600000, time1);
    TEST_ASSERT_UINT32_WITHIN(20000, 500000, time2);
}

void test_same_yieldyield()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_yield, (TaskFunction_t)busy_yield, (tskIDLE_PRIORITY + 1UL), (tskIDLE_PRIORITY + 1UL), &time1, &time2);

    // 600,000 +- 20,000 microseconds
    // (The shorter time for task2 is to account for the 100us delay before starting task2 after starting task1)
    // Accounting for this, the time of the two tasks should be roughly equal.
    TEST_ASSERT_UINT32_WITHIN(20000, 600000, time1);
    TEST_ASSERT_UINT32_WITHIN(20000, 500000, time2);
}

void test_same_busyyield()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_busy, (TaskFunction_t)busy_yield, (tskIDLE_PRIORITY + 1UL), (tskIDLE_PRIORITY + 1UL), &time1, &time2);

    // The task that yields should run for significantly less time, such that it can be approximated to 0 with some error.
    TEST_ASSERT_UINT32_WITHIN(200000, 1000000, time1);
    TEST_ASSERT_UINT32_WITHIN(200000, 0, time2);
}

void test_highfirst_busybusy()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_busy, (TaskFunction_t)busy_busy, (tskIDLE_PRIORITY + 2UL), (tskIDLE_PRIORITY + 1UL), &time1, &time2);

    // The lower priority task should be starved and never run.
    TEST_ASSERT_UINT32_WITHIN(200000, 1000000, time1);
    TEST_ASSERT_EQUAL_UINT32(0, time2);
}

void test_lowfirst_busybusy()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_busy, (TaskFunction_t)busy_busy, (tskIDLE_PRIORITY + 1UL), (tskIDLE_PRIORITY + 2UL), &time1, &time2);

    // The lower priority task should only run before it's starved by the higher priority task. This will be approximately the delay before 
    // higher priority task was started
    TEST_ASSERT_UINT32_WITHIN(20000, 1000000, time2);
    TEST_ASSERT_UINT32_WITHIN(20000, 100000, time1);
}

void test_diff_yieldyield()
{
    uint32_t time1, time2;

    get_thread_stats((TaskFunction_t)busy_yield, (TaskFunction_t)busy_yield, (tskIDLE_PRIORITY + 1UL), (tskIDLE_PRIORITY + 2UL), &time1, &time2);

    // The lower priority task should only run before it's starved by the higher priority task. This will be approximately the delay before 
    // higher priority task was started
    TEST_ASSERT_UINT32_WITHIN(20000, 1000000, time2);
    TEST_ASSERT_UINT32_WITHIN(20000, 100000, time1);
}

void runner(void)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_same_busybusy);
        RUN_TEST(test_same_yieldyield);
        RUN_TEST(test_same_busyyield);
        RUN_TEST(test_lowfirst_busybusy);
        RUN_TEST(test_highfirst_busybusy);
        RUN_TEST(test_diff_yieldyield);
        printf("\n");
        UNITY_END();
    }
}

int main (void)
{
    stdio_init_all();
    xTaskCreate((TaskFunction_t)runner, "Runner",
                TASK_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 3UL), NULL);
    vTaskStartScheduler();
}
