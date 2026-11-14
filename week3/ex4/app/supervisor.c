/* Week 3 / Ex4 -- supervisor task: waits for all monitored tasks to check in
 * via an event group every cycle; refreshes IWDG only if they all did.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "main.h"
#include "params.h"
#include "supervisor.h"

#include <stdio.h>

extern UART_HandleTypeDef huart1;
extern volatile uint32_t alpha_wait_ms;
extern volatile uint32_t beta_wait_ms;

EventGroupHandle_t supervisor_events;
extern IWDG_HandleTypeDef hiwdg;

#define ALL_BITS ((1u << SUPERVISOR_BITS) - 1u)

void supervisor_init(void)
{
    /* TODO 1: supervisor_events = xEventGroupCreate(); check non-NULL.
     *
     * TODO 2: HAL IWDG init here (IWDG_TIMEOUT_MS -> prescaler + reload per
     *         RM0008 IWDG chapter; IWDG runs off LSI, ~40 kHz, compute the
     *         reload value and show the computation in REPORT.md). */

    supervisor_events = xEventGroupCreate();
    configASSERT(supervisor_events != NULL);
}

void supervisor_task(void *argument)
{
    (void)argument;

    for (;;) {
        /* TODO 3: xEventGroupWaitBits(supervisor_events, ALL_BITS, pdTRUE,
         *         pdTRUE, <timeout>) -- clear-on-exit, wait-for-all. Pick a
         *         timeout tighter than IWDG_TIMEOUT_MS but loose enough that
         *         normal scheduling jitter does not cause a false trip;
         *         justify the number in REPORT.md.
         *
         * TODO 4: if the wait actually collected ALL_BITS (check the return
         *         value against ALL_BITS, do not assume success just because
         *         the call returned), HAL_IWDG_Refresh(&hiwdg). Otherwise do
         *         NOTHING -- that is what lets IWDG_TIMEOUT_MS expire and
         *         reset the board when a task stops checking in. */
        EventBits_t bits = xEventGroupWaitBits(
            supervisor_events,
            ALL_BITS,
            pdTRUE,
            pdTRUE,
            pdMS_TO_TICKS(750)
        );

        if ((bits & ALL_BITS) == ALL_BITS) {
            HAL_IWDG_Refresh(&hiwdg);
        } else {
    // Lúc cả Alpha và Beta đã bắt đầu chờ mutex thứ hai
    uint32_t deadlock_start =
        (alpha_wait_ms > beta_wait_ms)
        ? alpha_wait_ms : beta_wait_ms;

    for (;;) {
        uint32_t elapsed = HAL_GetTick() - deadlock_start;

        char msg[80];
        int n = snprintf(
            msg,
            sizeof(msg),
            "DEADLOCK TIME: %lu ms\r\n",
            (unsigned long)elapsed
        );

        if (n > 0 && n < (int)sizeof(msg)) {
            HAL_UART_Transmit(
                &huart1,
                (uint8_t *)msg,
                (uint16_t)n,
                50
            );
        }

        // Không refresh IWDG, chờ watchdog reset.
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
    }
}
