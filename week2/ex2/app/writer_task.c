/* Week 2 / Ex2 — WRITER_COUNT identical writer tasks. Each prints an MSG_LEN
 * byte string with its own id + sequence number, every WRITER_PERIOD_MS.
 */
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include <string.h>

#include "main.h"
#include "params.h"
#include "uart_guard.h"


/* TODO 1: one writer_task(void *argument) taking the writer index via the
 *         task parameter (argument), NOT WRITER_COUNT copy-pasted bodies.
 *         Format a MSG_LEN-byte string containing the writer id and a
 *         per-writer sequence number, then uart_guard_send() it, then
 *         vTaskDelay(pdMS_TO_TICKS(WRITER_PERIOD_MS)). */
void writer_task(void *argument)
{
    uint32_t id = (uint32_t)(uintptr_t)argument;
    uint32_t seq = 0;
    configASSERT(id < WRITER_COUNT);
    char msg[MSG_LEN];
    

    uint32_t samples = MEASURE_TOTAL_MESSAGES / WRITER_COUNT;
    if (id < (MEASURE_TOTAL_MESSAGES % WRITER_COUNT))
    {
        samples++;
    }



    for (uint32_t i = 0; i < samples; i++) {
    //for (;;){

        /* TODO 2 */
        seq++;
        memset(msg, 'A' + id, MSG_LEN);

        snprintf(msg, MSG_LEN - 2,
                "W%lu SEQ=%06lu",
                (unsigned long)id,
                (unsigned long)seq);

        size_t n = strlen(msg);
        msg[n] = ' ';

        msg[MSG_LEN - 2] = '\r';
        msg[MSG_LEN - 1] = '\n';

        uart_guard_send(msg, MSG_LEN);
        vTaskDelay(pdMS_TO_TICKS(WRITER_PERIOD_MS));
    }
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
