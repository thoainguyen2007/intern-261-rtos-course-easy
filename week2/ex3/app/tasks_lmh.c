/* Week 2 / Ex3 — priority inversion vs priority inheritance.
 *
 * task_low  (lowest prio)  holds the shared resource for LOW_HOLD_MS.
 * task_mid  (middle prio)  never touches the resource; pure CPU-bound work,
 *                          its only job is to be ready and preemptible so it
 *                          can steal CPU from task_low while task_low holds
 *                          the resource task_high is waiting for.
 * task_high (highest prio) needs the resource every HIGH_PERIOD_MS and is
 *                          the one whose block time we measure.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"
#include "params.h"
#include "tasks_lmh.h"

static SemaphoreHandle_t s_resource;

void resource_init(void)
{
#if RESOURCE_MODE == 0
    /* TODO 1: xSemaphoreCreateBinary(), then give it once so it starts
     *         "available" (binary semaphores start empty). */
    s_resource = xSemaphoreCreateBinary();
    configASSERT(s_resource != NULL);
    xSemaphoreGive(s_resource);

#else
    /* TODO 2: xSemaphoreCreateMutex(). */
    s_resource = xSemaphoreCreateMutex();
    configASSERT(s_resource != NULL);
#endif
}
/* DWT cycle counter — the only honest way to burn a known number of
 * microseconds of CPU inside a task without blocking.
 */
#include "stm32f1xx.h"
#include "FreeRTOSConfig.h"
#include <stdint.h>

void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void dwt_busy_wait_ms(uint32_t ms)
{
    uint32_t cycles = ms * (72000UL);
    uint32_t start = DWT->CYCCNT;
    while ((uint32_t)(DWT->CYCCNT - start) < cycles);
}

void task_low(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO 3: take s_resource (portMAX_DELAY), toggle a GPIO marking
         *         "resource held", busy-wait LOW_HOLD_MS using the ex2 DWT
         *         helper or a HAL tick-based wait, lower the GPIO, give
         *         s_resource back. Then yield/delay briefly before looping —
         *         this task must not monopolize the CPU outside the hold. */

        if (xSemaphoreTake(s_resource, portMAX_DELAY)==pdTRUE){

                GPIOA->BSRR = 1;
                dwt_busy_wait_ms(LOW_HOLD_MS);
                GPIOA->BRR = 1;

            xSemaphoreGive(s_resource);
        }
        // taskYIELD();
        vTaskDelay(pdMS_TO_TICKS(1));
        
    }
}

void task_mid(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO 4: toggle a GPIO, busy-wait MID_WORK_MS, lower the GPIO, then
         *         vTaskDelay a short period so it keeps becoming Ready
         *         periodically rather than running once and blocking
         *         forever. This task never touches s_resource. */
        GPIOA->BSRR = 1U<<1;
        dwt_busy_wait_ms(MID_WORK_MS);
        GPIOA->BRR = 1U<<1;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void task_high(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        /* TODO 5: toggle a GPIO high (marks "task_high wants the resource"),
         *         take s_resource (portMAX_DELAY), toggle a second GPIO
         *         transition (marks "task_high got it"), do a small amount
         *         of work, give s_resource back, lower the first GPIO.
         *         The gap between the two GPIO edges on the logic analyzer
         *         is your block time t1 - t0. */
        GPIOA->BSRR = 1U<<2;
        if (xSemaphoreTake(s_resource, portMAX_DELAY)==pdTRUE){

            GPIOA->BSRR = 1U<<3;
            dwt_busy_wait_ms(1);
            GPIOA->BRR = 1U<<3;
            xSemaphoreGive(s_resource);
            GPIOA->BRR = 1U<<2;

        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(HIGH_PERIOD_MS));
    }
}
