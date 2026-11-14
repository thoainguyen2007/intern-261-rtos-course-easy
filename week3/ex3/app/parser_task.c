/* Week 3 / Ex3 -- command parser: rx_stream -> change demo_task's
 * period/priority/suspend state at runtime.
 *
 * Commands (each terminated by '\r', max MAX_CMD_LEN bytes):
 *   P<ms>\r   -- set demo_task period to <ms>
 *   S<prio>\r -- vTaskPrioritySet(demo_task, <prio>)
 *   T<0|1>\r  -- 0 = vTaskSuspend(demo_task), 1 = vTaskResume(demo_task)
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "main.h"
#include "params.h"
#include "uart_rx.h"
#include "parser_task.h"

extern UART_HandleTypeDef huart1;

static TaskHandle_t s_demo_task_handle;
static volatile uint32_t s_demo_period_ms = 200;

static uint32_t s_test_ok = 0;
static uint32_t s_last_seq = 0;
static uint32_t s_drop_start = 0;
static BaseType_t s_have_seq = pdFALSE;

static void apply_command(const char *cmd, size_t len)
{
    /* TODO 1: switch on cmd[0]: 'P' -> parse decimal into s_demo_period_ms
     *         (demo_task reads this every loop, no restart needed);
     *         'S' -> parse decimal, vTaskPrioritySet(s_demo_task_handle, n);
     *         'T' -> cmd[1]=='0' ? vTaskSuspend(...) : vTaskResume(...).
     *         Ignore anything malformed -- do not crash on bad input. */
    if (len < 2 || s_demo_task_handle == NULL)
        return;

    char *end;
    unsigned long value = strtoul(&cmd[1], &end, 10);

    if (end == &cmd[1] || *end != '\0')
        return;

    switch (cmd[0])
    {
        case 'P':
            if (value > 0 && value <= UINT32_MAX)
                s_demo_period_ms = (uint32_t)value;
            break;

        case 'S':
            if (value < configMAX_PRIORITIES)
                vTaskPrioritySet(s_demo_task_handle, (UBaseType_t)value);
            break;

        case 'T':
            if (len != 2)
                break;

            if (value == 0)
                vTaskSuspend(s_demo_task_handle);
            else if (value == 1)
                vTaskResume(s_demo_task_handle);
            break;

        default:
            break;
    }
}

void parser_task(void *argument)
{
    (void)argument;
    char cmd[MAX_CMD_LEN];
    size_t idx = 0;
    BaseType_t discard = pdFALSE;  // Thêm ở đây

    for (;;) {
        uint8_t byte;

        /* TODO 2: xStreamBufferReceive(rx_stream, &byte, 1, portMAX_DELAY). */
        if (xStreamBufferReceive(rx_stream, &byte, 1, portMAX_DELAY) != 1){
            continue;
        }

        /* TODO 3: if byte == '\r', call apply_command(cmd, idx), idx = 0.
         *         else if idx < MAX_CMD_LEN - 1, cmd[idx++] = byte.
         *         else (overflow): discard the whole partial command, idx = 0
         *         -- explain in REPORT.md why silently truncating instead
         *         would be worse. */
        if (byte == '\r')
        {
            if (discard == pdFALSE)
            {
                cmd[idx] = '\0';

if (strcmp(cmd, "Z") == 0)
{
    s_test_ok = 0;
    s_last_seq = 0;
    s_have_seq = pdFALSE;
    s_drop_start = uart_rx_get_dropped();

    const char msg[] = "READY\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)msg,
                      sizeof(msg) - 1, 100);
}
else if (strcmp(cmd, "R") == 0)
{
    char msg[80];

    int n = snprintf(
        msg, sizeof(msg),
        "TEST_OK=%lu DROP_BYTES=%lu\r\n",
        (unsigned long)s_test_ok,
        (unsigned long)(uart_rx_get_dropped() - s_drop_start)
    );

    if (n > 0 && n < (int)sizeof(msg))
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)msg,
                          (uint16_t)n, 100);
    }
}
else if (strncmp(cmd, "P10#", 4) == 0)
{
    const char *p = cmd + 4;
    BaseType_t valid = (*p != '\0') ? pdTRUE : pdFALSE;

    for (const char *c = p; *c != '\0'; c++)
    {
        if (*c < '0' || *c > '9')
        {
            valid = pdFALSE;
            break;
        }
    }

    if (valid == pdTRUE)
    {
        unsigned long seq = strtoul(p, NULL, 10);

        if (seq <= UINT32_MAX &&
            (s_have_seq == pdFALSE || seq > s_last_seq))
        {
            s_last_seq = (uint32_t)seq;
            s_have_seq = pdTRUE;
            s_test_ok++;

            apply_command("P10", 3);
        }
    }
}
else
{
    apply_command(cmd, idx);
}
            }

            idx = 0;
            discard = pdFALSE;
        }
        else if (discard == pdFALSE)
        {
            if (idx < MAX_CMD_LEN - 1)
            {
                cmd[idx++] = (char)byte;
            }
            else
            {
                discard = pdTRUE;
                idx = 0;
            }
        }
    }
}

void demo_task(void *argument)
{
    (void)argument;
    s_demo_task_handle = xTaskGetCurrentTaskHandle();

    for (;;) {
        /* TODO 4: toggle a GPIO pin here. */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        vTaskDelay(pdMS_TO_TICKS(s_demo_period_ms/2));
    }
}
