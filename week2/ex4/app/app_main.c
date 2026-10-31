/* Week 2 / Ex4 — event group & task notification.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "sync_tasks.h"

void app_main(void)
{
    sync_init();

    /* TODO 1: xTaskCreate sync_task first, store its handle into
     *         sync_task_handle (cast to void*) — producer_0 needs it when
     *         PRODUCER0_USE_NOTIFY is set. */

    TaskHandle_t handle = NULL;

    if (xTaskCreate(sync_task, "SYNC", 256, NULL, 3, &handle) == pdPASS)
    {
        sync_task_handle = (void *)handle;
    }


    /* TODO 2: xTaskCreate EVENT_BIT_COUNT instances of producer_task, each
     *         given its own index (0..EVENT_BIT_COUNT-1) as the task
     *         parameter. */

    for (uint32_t i = 0; i < EVENT_BIT_COUNT+1; i++)
    {
    if (xTaskCreate(
            producer_task,
            "PRODUCER",
            128,
            (void *)(uintptr_t)i,
            2,
            NULL
        ) != pdPASS)
    {
        for (;;);
    }
}

    vTaskStartScheduler();

    for (;;) {
    }
}
