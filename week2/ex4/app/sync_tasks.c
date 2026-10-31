
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "main.h"
#include "params.h"
#include "sync_tasks.h"

#include "core_cm3.h"
#include <stdio.h>
#include <stdint.h>

static EventGroupHandle_t s_events;
void *sync_task_handle;

#define ALL_BITS_MASK ((1U << (EVENT_BIT_COUNT+1)) - 1U)
#define REMAINING_BITS (ALL_BITS_MASK & ~(EventBits_t)1U)

/* DWT measurement */
static volatile uint32_t t0_cycles = 0;
static volatile uint8_t notify_waiting = 0;
static volatile uint8_t sample_ready = 0;

void sync_init(void)
{
    /* Initialize heap_4 before measuring */
    void *p = pvPortMalloc(1);
    vPortFree(p);

    size_t before = xPortGetFreeHeapSize();

    s_events = xEventGroupCreate();

    if (s_events == NULL)
    {
        for (;;);
    }

    size_t after = xPortGetFreeHeapSize();

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    printf("\r\n===== EX4 RAM =====\r\n");
    printf("HEAP_BEFORE=%lu\r\n", (unsigned long)before);
    printf("HEAP_AFTER=%lu\r\n", (unsigned long)after);
    printf("EVENT_GROUP_RAM=%lu\r\n",
           (unsigned long)(before - after));
    printf("NOTIFICATION_EXTRA_RAM=0\r\n");
    printf("===================\r\n");
}

void producer_task(void *argument)
{
    uint32_t idx = (uint32_t)(uintptr_t)argument;
    uint32_t seed = 12345U + idx * 1000U;

    for (;;)
    {
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;

        uint32_t delay_ms =
            NOTIFY_PERIOD_MS + (seed % 5U);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));

#if PRODUCER0_USE_NOTIFY
        if (idx == 0)
        {
            if (notify_waiting)
            {
                t0_cycles = DWT->CYCCNT;
                sample_ready = 1;
            }

            xTaskNotifyGive(
                (TaskHandle_t)sync_task_handle
            );
        }
        else
#endif
        {
            xEventGroupSetBits(s_events, 1U << idx);
        }
    }
}

void sync_task(void *argument)
{
    (void)argument;

    TickType_t last_sync = xTaskGetTickCount();
    uint32_t count = 0;
    uint32_t period_sum = 0;

    for (;;)
    {
        EventBits_t bits = xEventGroupWaitBits(
            s_events,
            ALL_BITS_MASK,
            pdTRUE,
            pdTRUE,
            pdMS_TO_TICKS(SYNC_TIMEOUT_MS)
        );

        if ((bits & ALL_BITS_MASK) == ALL_BITS_MASK)
        {
            TickType_t now = xTaskGetTickCount();

            period_sum +=
                (now - last_sync) * portTICK_PERIOD_MS;

            last_sync = now;
            count++;

            if (count == 20)
            {
                uint32_t avg_x100 =
                    period_sum * 100U / count;

                printf("\r\n===== EX4 5 PRODUCERS =====\r\n");
                printf("SYNC_SAMPLES=%lu\r\n",
                       (unsigned long)count);
                printf("SYNC_AVG_MS=%lu.%02lu\r\n",
                       (unsigned long)(avg_x100 / 100U),
                       (unsigned long)(avg_x100 % 100U));
                printf("=========================\r\n");

                count = 0;
                period_sum = 0;
            }
        }
    }
}