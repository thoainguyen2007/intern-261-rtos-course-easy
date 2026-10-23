/* Week 1 / Ex4 — print the live RAM picture over USART1.
 *
 * This is your measurement instrument. It must be honest: read the values from
 * FreeRTOS, never from a constant you typed in.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "main.h"
#include "params.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

/* Handle của dyn_task và stat_task được export từ app_main.c */
extern TaskHandle_t xDynTaskHandle;
extern TaskHandle_t xStatTaskHandle;

/* TODO 1: a blocking UART print helper. It is called from a task, so it may
 *         block — but say in REPORT.md what would break if you called it from
 *         the stack overflow hook instead. */
static void uart_print(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void ram_report_task(void *argument)
{
    (void)argument;
    char buffer[128];

    for (;;) {
        /* TODO 2: print, every 2 seconds:
         *   - xPortGetFreeHeapSize()
         *   - xPortGetMinimumEverFreeHeapSize()
         *   - uxTaskGetStackHighWaterMark() for: dyn_task, stat_task,
         *     ram_report_task, the idle task (xTaskGetIdleTaskHandle()),
         *     and the timer task (xTimerGetTimerDaemonTaskHandle()).
         *
         * High water mark is returned in WORDS. Print both words and bytes so
         * your report cannot silently mix the two units. */

        uart_print("\r\n================ FREE RTOS RAM PICTURE ================\r\n");

        /* 1. Đo dung lượng Heap */
        size_t free_heap = xPortGetFreeHeapSize();
        size_t min_ever_free = xPortGetMinimumEverFreeHeapSize();

        snprintf(buffer, sizeof(buffer), "Free Heap          : %u bytes\r\n", (unsigned int)free_heap);
        uart_print(buffer);
        snprintf(buffer, sizeof(buffer), "Min Ever Free Heap : %u bytes\r\n", (unsigned int)min_ever_free);
        uart_print(buffer);

        UBaseType_t hwm_words;

        /* 2. Đo High Water Mark của dyn_task */
        if (xDynTaskHandle != NULL) {
            hwm_words = uxTaskGetStackHighWaterMark(xDynTaskHandle);
            snprintf(buffer, sizeof(buffer), "HWM dyn_task       : %3u words (%4u bytes)\r\n",
                     (unsigned int)hwm_words, (unsigned int)(hwm_words * sizeof(StackType_t)));
            uart_print(buffer);
        }

        /* 3. Đo High Water Mark của stat_task */
        if (xStatTaskHandle != NULL) {
            hwm_words = uxTaskGetStackHighWaterMark(xStatTaskHandle);
            snprintf(buffer, sizeof(buffer), "HWM stat_task      : %3u words (%4u bytes)\r\n",
                     (unsigned int)hwm_words, (unsigned int)(hwm_words * sizeof(StackType_t)));
            uart_print(buffer);
        }

        /* 4. Đo High Water Mark của chính ram_report_task (truyền NULL đại diện cho task hiện tại) */
        hwm_words = uxTaskGetStackHighWaterMark(NULL);
        snprintf(buffer, sizeof(buffer), "HWM report_task    : %3u words (%4u bytes)\r\n",
                 (unsigned int)hwm_words, (unsigned int)(hwm_words * sizeof(StackType_t)));
        uart_print(buffer);

        /* 5. Đo High Water Mark của Idle Task */
        TaskHandle_t xIdleHandle = xTaskGetIdleTaskHandle();
        if (xIdleHandle != NULL) {
            hwm_words = uxTaskGetStackHighWaterMark(xIdleHandle);
            snprintf(buffer, sizeof(buffer), "HWM idle_task      : %3u words (%4u bytes)\r\n",
                     (unsigned int)hwm_words, (unsigned int)(hwm_words * sizeof(StackType_t)));
            uart_print(buffer);
        }

        /* 6. Đo High Water Mark của Timer Task */
        TaskHandle_t xTimerHandle = xTimerGetTimerDaemonTaskHandle();
        if (xTimerHandle != NULL) {
            hwm_words = uxTaskGetStackHighWaterMark(xTimerHandle);
            snprintf(buffer, sizeof(buffer), "HWM timer_task     : %3u words (%4u bytes)\r\n",
                     (unsigned int)hwm_words, (unsigned int)(hwm_words * sizeof(StackType_t)));
            uart_print(buffer);
        }

        uart_print("=======================================================\r\n");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}