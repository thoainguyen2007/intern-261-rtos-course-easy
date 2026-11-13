/* Week 3 / Ex2 -- debounce via one-shot timer + long-press via auto-reload
 * timer. Native FreeRTOS API only.
 */
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "main.h"
#include "params.h"
#include "debounce.h"


extern UART_HandleTypeDef huart1;
static TimerHandle_t s_debounce_timer;
static TimerHandle_t s_longpress_timer;

static void debounce_callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    /* TODO 1: printf/HAL_UART "PRESS" + xTaskGetTickCount(). This is the
     *         stable-press event, DEBOUNCE_MS after the last edge.
     *
     * TODO 2 (temporary, for the README step-4 experiment ONLY): add
     *         vTaskDelay(pdMS_TO_TICKS(50)) here, capture what happens to
     *         longpress_timer's callback timing in your REPORT.md, then
     *         DELETE this line again before you submit. A callback that
     *         blocks here is blocking the ONE daemon task every other
     *         software timer in the system also depends on. */
    char msg[48];

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        // Nut da nhan on dinh
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

        snprintf(msg, sizeof(msg), "PRESS tick=%lu\r\n",
                 (unsigned long)xTaskGetTickCount());

        HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    }
    else
    {
        // Nut da tha
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    }
    // vTaskDelay(pdMS_TO_TICKS(50));
}

static void longpress_callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    /* TODO 3: printf/HAL_UART "HELD" + xTaskGetTickCount(). Fires once per
     *         LONGPRESS_MS while the button stays down. */
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        char msg[48];

        snprintf(msg, sizeof(msg), "HELD tick=%lu\r\n",
                 (unsigned long)xTaskGetTickCount());

        HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    }
}

void debounce_init(void)
{
    /* TODO 4: s_debounce_timer = xTimerCreate(..., pdMS_TO_TICKS(DEBOUNCE_MS),
     *         pdFALSE, NULL, debounce_callback); -- pdFALSE = one-shot.
     *         s_longpress_timer = xTimerCreate(..., pdMS_TO_TICKS(LONGPRESS_MS),
     *         pdTRUE, NULL, longpress_callback); -- pdTRUE = auto-reload.
     *         Check both handles non-NULL. Do NOT xTimerStart() either here --
     *         they are driven entirely from debounce_on_edge(). */
    s_debounce_timer = xTimerCreate(
        "debounce",
        pdMS_TO_TICKS(DEBOUNCE_MS),
        pdFALSE,
        NULL,
        debounce_callback
    );

    s_longpress_timer = xTimerCreate(
        "longpress",
        pdMS_TO_TICKS(LONGPRESS_MS),
        pdTRUE,
        NULL,
        longpress_callback
    );

    configASSERT(s_debounce_timer != NULL);
    configASSERT(s_longpress_timer != NULL);
}

void debounce_on_edge(uint8_t pin_is_high)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* TODO 5: xTimerResetFromISR(s_debounce_timer, &xHigherPriorityTaskWoken)
     *         on every edge -- this is what swallows bounce: each new edge
     *         restarts the DEBOUNCE_MS countdown, so the callback only fires
     *         once the line has been quiet for DEBOUNCE_MS. */
    xTimerResetFromISR(
        s_debounce_timer,
        &xHigherPriorityTaskWoken
    );

    /* TODO 6: if (pin_is_high) xTimerStartFromISR(s_longpress_timer, ...);
     *         else xTimerStopFromISR(s_longpress_timer, ...); */

    if (!pin_is_high)
    {
        xTimerStartFromISR(
            s_longpress_timer,
            &xHigherPriorityTaskWoken
        );
    }
    else
    {
        xTimerStopFromISR(
            s_longpress_timer,
            &xHigherPriorityTaskWoken
        );
    }

    /* TODO 7: portYIELD_FROM_ISR(xHigherPriorityTaskWoken). */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


    (void)pin_is_high;
    (void)xHigherPriorityTaskWoken;
}



/* Ket noi EXTI voi debounce */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_1)
    {
        uint8_t pin_is_high =
            (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET);

        debounce_on_edge(pin_is_high);
    }
}