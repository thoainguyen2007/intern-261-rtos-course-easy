/* Week 2 / Ex1 — sensor pipeline: sensor_task -> queue_raw -> filter_task ->
 * queue_filtered -> uart_task.
 *
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"
#include "params.h"
#include "pipeline.h"

QueueHandle_t queue_raw;
QueueHandle_t queue_filtered;







#include <stdio.h>

extern UART_HandleTypeDef huart1;

#define LOG_CAPACITY 256

TaskHandle_t uart_handle = NULL;

typedef struct {
    uint32_t time_ms;
    uint8_t raw_depth;
    uint8_t filtered_depth;
} depth_sample_t;

static depth_sample_t depth_log[LOG_CAPACITY];



void depth_logger_task(void *argument)
{
    (void)argument;

    TickType_t start = xTaskGetTickCount();
    TickType_t last_wake = start;

    // Đo 256 lần, chu kỳ mục tiêu 10 ms
    for (uint32_t i = 0; i < LOG_CAPACITY; i++)
    {
        TickType_t now = xTaskGetTickCount();

        depth_log[i].time_ms =
            (uint32_t)((now - start) * portTICK_PERIOD_MS);

        depth_log[i].raw_depth =
            (uint8_t)uxQueueMessagesWaiting(queue_raw);

        depth_log[i].filtered_depth =
            (uint8_t)uxQueueMessagesWaiting(queue_filtered);

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(10));
    }

    // Ngừng UART task trước khi xuất CSV
    vTaskSuspend(uart_handle);

    // Truyền UART đang thực hiện có thể bị ngắt giữa chừng.
    // Khởi tạo lại UART để logger sử dụng.
    HAL_UART_Abort(&huart1);

    char line[48];
    const char header[] =
        "\r\nCSV_BEGIN\r\n"
        "time_ms,raw_depth,filtered_depth\r\n";

    HAL_UART_Transmit(
        &huart1, (uint8_t *)header,
        sizeof(header) - 1, HAL_MAX_DELAY
    );

    for (uint32_t i = 0; i < LOG_CAPACITY; i++)
    {
        int len = snprintf(
            line, sizeof(line), "%lu,%u,%u\r\n",
            (unsigned long)depth_log[i].time_ms,
            (unsigned int)depth_log[i].raw_depth,
            (unsigned int)depth_log[i].filtered_depth
        );

        if (len > 0 && len < (int)sizeof(line))
        {
            HAL_UART_Transmit(
                &huart1, (uint8_t *)line,
                (uint16_t)len, HAL_MAX_DELAY
            );
        }
    }

    const char footer[] = "CSV_END\r\n";

    HAL_UART_Transmit(
        &huart1, (uint8_t *)footer,
        sizeof(footer) - 1, HAL_MAX_DELAY
    );

    vTaskDelete(NULL);
}


void app_main(void)
{
    /* TODO 1: xQueueCreate(QUEUE_LENGTH, sizeof(raw_sample_t)) for queue_raw,
     *         and the equivalent for queue_filtered with filtered_sample_t.
     *         Check both handles are non-NULL before continuing. */
    queue_raw=xQueueCreate(QUEUE_LENGTH, sizeof(raw_sample_t));
    queue_filtered=xQueueCreate(QUEUE_LENGTH, sizeof(filtered_sample_t));
    if (queue_raw==NULL || queue_filtered==NULL){ 
        taskDISABLE_INTERRUPTS();
        for (;;);
    }


    /* TODO 2: xTaskCreate sensor_task, filter_task, uart_task.
     *         Pick priorities you can justify in REPORT.md: who must never be
     *         starved by whom? */
    xTaskCreate(
        sensor_task,
        "sensor",
        512,
        NULL,
        4,
        NULL
    );
    xTaskCreate(
        filter_task,
        "filter",
        512,
        NULL,
        3,
        NULL
    );
    xTaskCreate(
        uart_task,
        "uart",
        512,
        NULL,
        2,
        // NULL
        &uart_handle
    );
    /* TODO 3: xTaskCreate depth_logger_task — must not perturb the pipeline it
     *         is measuring (think about its own priority and period). */
    xTaskCreate(
        depth_logger_task,
        "logger",
        512,
        NULL,
        2,
        NULL
    );
    vTaskStartScheduler();

    for (;;) {
    }
}

