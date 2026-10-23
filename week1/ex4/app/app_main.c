/* Week 1 / Ex4 — entry point. */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"

void ram_report_task(void *argument);

/* TODO 1: static storage for stat_task — TCB and stack.
 *         The stack array type matters; it is not char[]. */
static StaticTask_t xStatTaskTCB;
static StackType_t  uxStatTaskStack[STATIC_TASK_STACK_WORDS];

/* Export handle để ram_report_task có thể đọc High Water Mark */
TaskHandle_t xDynTaskHandle = NULL;
TaskHandle_t xStatTaskHandle = NULL;

static void shared_work(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO 2: identical trivial work in both tasks, so that any difference
         *         in high water mark comes from how they were created, not from
         *         what they do. */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
static void dyn_overflow_work(void *argument)
{
    (void)argument;

    /* Khai báo mảng đúng bằng HOG_DEPTH_BYTES (640 bytes) trên stack */
    volatile uint8_t hog_array[HOG_DEPTH_BYTES];

    /* Ghi dữ liệu vào từng byte để compiler -O2 không tối ưu bỏ qua */
    for (int i = 0; i < HOG_DEPTH_BYTES; i++) {
        hog_array[i] = (uint8_t)(i & 0xFF);
    }

    // for (;;) {
    //     vTaskDelay(pdMS_TO_TICKS(50));
    // }
}

void app_main(void)
{
    /* TODO 3: dyn_task  via xTaskCreate,       stack DYN_TASK_STACK_WORDS
     *         stat_task via xTaskCreateStatic, stack STATIC_TASK_STACK_WORDS
     *         ram_report_task via xTaskCreate
     *
     * Check every return value. xTaskCreateStatic returns a handle, not a
     * BaseType_t — explain the difference in REPORT.md. */

    //  BaseType_t xDynRet = xTaskCreate(
    //     dyn_overflow_work,
    //     "tran_stack",
    //     DYN_TASK_STACK_WORDS,
    //     NULL,
    //     tskIDLE_PRIORITY + 1,
    //     &xDynTaskHandle
    // );
    // configASSERT(xDynRet == pdPASS);


    BaseType_t xDynRet = xTaskCreate(
        shared_work,
        "dyn_task",
        // DYN_TASK_STACK_WORDS,
        28,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xDynTaskHandle
    );
    configASSERT(xDynRet == pdPASS);

    xStatTaskHandle = xTaskCreateStatic(
        shared_work,
        "stat_task",
        STATIC_TASK_STACK_WORDS,
        NULL,
        tskIDLE_PRIORITY + 1,
        uxStatTaskStack,
        &xStatTaskTCB
    );
    configASSERT(xStatTaskHandle != NULL);

    BaseType_t xRepRet = xTaskCreate(
        ram_report_task,
        "ram_report",
        256,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL
    );
    configASSERT(xRepRet == pdPASS);

    /* TODO 4 (step 6 of the README, enable only for that run):
     *         declare a HOG_DEPTH_BYTES local array inside dyn_task's work
     *         function and write to every element, so the overflow is real and
     *         not optimised away. Think about what -O2 does to a dead array. */


    vTaskStartScheduler();

    /* Scheduler returned => not enough heap for the idle/timer task. */
    for (;;) {
    }
}
