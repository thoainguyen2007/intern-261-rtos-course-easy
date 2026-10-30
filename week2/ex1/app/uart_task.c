/* Week 2 / Ex1 — prints filtered samples to USART1 at UART_BAUD.
 * queue_filtered -> UART.
 *
 * This is also the task you deliberately starve in step 7 (temporarily set
 * UART_BAUD-equivalent hardware baud to 9600) to force queue_filtered to fill.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

#include "main.h"
#include "params.h"
#include "pipeline.h"

#include <stdint.h>

extern UART_HandleTypeDef huart1;

void uart_task(void *argument)
{
    (void)argument;

    for (;;) {
        filtered_sample_t in;

        /* TODO 1: xQueueReceive(queue_filtered, &in, portMAX_DELAY). */
        xQueueReceive(queue_filtered, &in, portMAX_DELAY);
        static uint8_t measured = 0;


        //Câu 6 report
if (!measured)
{
    measured = 1;

    uint32_t tick = HAL_GetTick();
    char msg[100];

    int n = snprintf(
        msg, sizeof(msg),
        "\r\nFILTER_WINDOW=%d, FIRST_SEQ=%lu, FIRST_UART_MS=%lu\r\n",
        FILTER_WINDOW,
        (unsigned long)in.seq,
        (unsigned long)tick
    );

    if (n > 0 && n < (int)sizeof(msg))
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)msg, n, HAL_MAX_DELAY);
    }
}


        /* TODO 2: format "seq,ax,ay,az\r\n" and HAL_UART_Transmit on USART1.
         *         Transmit time at UART_BAUD is the thing that makes this
         *         task slow relative to sensor_task/filter_task — say so in
         *         REPORT.md with the actual byte time you computed. */
        char buffer[64];

        int len = snprintf(
            buffer,
            sizeof(buffer),
            "%lu,%d,%d,%d\r\n",
            (unsigned long)in.seq,
            (int)in.ax,
            (int)in.ay,
            (int)in.az
        );

        if (len > 0 && len < (int)sizeof(buffer))
        {
            HAL_UART_Transmit(
                &huart1,
                (uint8_t *)buffer,
                (uint16_t)len,
                HAL_MAX_DELAY
            );
        }
    }
}
