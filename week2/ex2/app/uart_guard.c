/* Week 2 / Ex2 — four ways to serialize access to USART1 from WRITER_COUNT
 * tasks. Only one UART_GUARD_MODE is compiled at a time.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "main.h"
#include "params.h"
#include "uart_guard.h"

#include <stdio.h>

extern UART_HandleTypeDef huart1;

#if UART_GUARD_MODE == 1
static SemaphoreHandle_t s_uart_mutex;
#elif UART_GUARD_MODE == 3
static QueueHandle_t s_gatekeeper_queue;
#endif





/**************Đo latency**********************/
typedef struct
{
    uint64_t total_cycles;
    uint32_t max_cycles;
    uint32_t critical_max_cycles;
    uint32_t count;
    uint32_t errors;
} LatencyStats;


static LatencyStats s_stats = {0};
static volatile uint32_t s_completed = 0;
static size_t s_ram_added = 0;
static size_t s_heap_before = 0;
static size_t s_heap_after = 0;


#if UART_GUARD_MODE == 3
#define MAX_SAMPLES_PER_WRITER \
    ((MEASURE_TOTAL_MESSAGES + WRITER_COUNT - 1) / WRITER_COUNT)

static uint32_t s_start_cycles
    [WRITER_COUNT][MAX_SAMPLES_PER_WRITER];
#endif

static void latency_record(
    uint32_t start,
    uint32_t end,
    BaseType_t success)
{
    uint32_t cycles = end - start;

    taskENTER_CRITICAL();

    if (success == pdTRUE)
    {
        s_stats.total_cycles += cycles;
        s_stats.count++;

        if (cycles > s_stats.max_cycles)
        {
            s_stats.max_cycles = cycles;
        }
    }
    else
    {
        s_stats.errors++;
    }

    s_completed++;

    taskEXIT_CRITICAL();
}
#if UART_GUARD_MODE == 3
static void get_message_info(
    const char *buf,
    uint32_t *id,
    uint32_t *seq)
{
    *id = (uint32_t)(buf[1] - '0');
    *seq = 0;

    for (uint32_t i = 7; i < 13; i++)
    {
        *seq = (*seq * 10U)
             + (uint32_t)(buf[i] - '0');
    }
}
#endif
/************************************/



void uart_guard_init(void)
{
    void *warmup = pvPortMalloc(1);
    configASSERT(warmup != NULL);
    vPortFree(warmup);
    
    size_t before = xPortGetFreeHeapSize();


#if UART_GUARD_MODE == 1
    /* TODO 1: xSemaphoreCreateMutex(). Check for NULL. */
    s_uart_mutex=xSemaphoreCreateMutex();
    configASSERT(s_uart_mutex != NULL);




#elif UART_GUARD_MODE == 3
    /* TODO 2: create s_gatekeeper_queue (length: pick and justify in
     *         REPORT.md), item size = MSG_LEN bytes.
     *         xTaskCreate(gatekeeper_task, ..., GATEKEEPER_PRIO). */
    s_gatekeeper_queue = xQueueCreate(
        8,
        MSG_LEN
    );

    configASSERT(s_gatekeeper_queue != NULL);

    BaseType_t result = xTaskCreate(
        gatekeeper_task,
        "Gatekeeper",
        256,
        NULL,
        GATEKEEPER_PRIO,
        NULL
    );

    configASSERT(result == pdPASS);


#endif


size_t after = xPortGetFreeHeapSize();

s_heap_before = before;
s_heap_after = after;

s_ram_added = (before >= after) ? (before - after) : 0;
}

static void msg_send(const char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        HAL_UART_Transmit(
            &huart1,
            (uint8_t *)&buf[i],
            1U,
            HAL_MAX_DELAY
        );
        #if UART_GUARD_MODE != 2
            taskYIELD();
        #endif
    }
}

void uart_guard_send(const char *buf, size_t len)
{
#if UART_GUARD_MODE == 0
    /* The bug: no serialization at all. */
    /* TODO 3: HAL_UART_Transmit directly. */
    msg_send(buf, len);

#elif UART_GUARD_MODE == 1
    /* TODO 4: xSemaphoreTake(s_uart_mutex, portMAX_DELAY), transmit, Give.
     *         Every return path must Give — no early return between Take and
     *         Give. */
    uint32_t start = DWT->CYCCNT;
    if (xSemaphoreTake(s_uart_mutex, portMAX_DELAY)==pdTRUE){
        msg_send(buf,len);


        uint32_t end = DWT->CYCCNT;
        xSemaphoreGive(s_uart_mutex);

        latency_record(start, end, pdTRUE);
    }

#elif UART_GUARD_MODE == 2
    /* TODO 5: taskENTER_CRITICAL(), transmit, taskEXIT_CRITICAL().
     *         Measure in REPORT.md how long the critical section holds IRQs
     *         off, as a function of MSG_LEN and UART_BAUD. */


    uint32_t start = DWT->CYCCNT;
    taskENTER_CRITICAL();

    uint32_t critical_start = DWT->CYCCNT;
    msg_send(buf,len);
    uint32_t critical_end = DWT->CYCCNT;
    taskEXIT_CRITICAL();
    
    
    
    uint32_t end = DWT->CYCCNT;

    latency_record(start, end, pdTRUE);

    uint32_t critical_cycles = critical_end - critical_start;

    taskENTER_CRITICAL();

    if (critical_cycles > s_stats.critical_max_cycles)
    {
        s_stats.critical_max_cycles = critical_cycles;
    }

    taskEXIT_CRITICAL();


#elif UART_GUARD_MODE == 3
    /* TODO 6: xQueueSend(s_gatekeeper_queue, buf, portMAX_DELAY).
     *         The writer never touches UART directly in this mode. */

    uint32_t id, seq;
    get_message_info(buf, &id, &seq);

    configASSERT(id < WRITER_COUNT);
    configASSERT(seq >= 1 && seq <= MAX_SAMPLES_PER_WRITER);

    s_start_cycles[id][seq - 1] = DWT->CYCCNT;




    xQueueSend(s_gatekeeper_queue, buf, portMAX_DELAY);



#endif
}

#if UART_GUARD_MODE == 3
void gatekeeper_task(void *argument)
{
    (void)argument;
    for (;;) {
        char msg[MSG_LEN];
        /* TODO 7: xQueueReceive(s_gatekeeper_queue, msg, portMAX_DELAY), then
         *         HAL_UART_Transmit. This is the only task allowed to call
         *         HAL_UART_Transmit in this mode. */
        if (xQueueReceive(s_gatekeeper_queue, msg, portMAX_DELAY) == pdPASS) {
            uint32_t id, seq;
            get_message_info(msg, &id, &seq);

            configASSERT(id < WRITER_COUNT);
            configASSERT(seq >= 1 && seq <= MAX_SAMPLES_PER_WRITER);

            uint32_t start = s_start_cycles[id][seq - 1];

            msg_send(msg, MSG_LEN);

            uint32_t end = DWT->CYCCNT;

            latency_record(start, end, pdTRUE);
        }
        
    }
}
#endif
void latency_report_task(void *argument)
{
    (void)argument;

    while (s_completed < MEASURE_TOTAL_MESSAGES)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    uint32_t avg_us = 0;
    uint32_t max_us = 0;
    uint32_t critical_us = 0;

    if (s_stats.count > 0)
    {
        avg_us = (uint32_t)(
            (s_stats.total_cycles / s_stats.count)
            * 1000000ULL / SystemCoreClock
        );

        max_us = (uint32_t)(
            (uint64_t)s_stats.max_cycles
            * 1000000ULL / SystemCoreClock
        );
    }

    critical_us = (uint32_t)(
        (uint64_t)s_stats.critical_max_cycles
        * 1000000ULL / SystemCoreClock
    );

    char report[200];

    int n = snprintf(
        report, sizeof(report),
        "\r\n===== EX2 REPORT =====\r\n"
        "MODE=%u WRITERS=%u\r\n"
        "SAMPLES=%lu ERRORS=%lu\r\n"
        "AVG_US=%lu\r\n"
        "MAX_US=%lu\r\n"
        "HEAP_BEFORE=%lu\r\n"
        "HEAP_AFTER=%lu\r\n"    
        "RAM_ADDED=%lu\r\n"
        "CRITICAL_US=%lu\r\n"
        "======================\r\n",
        (unsigned int)UART_GUARD_MODE,
        (unsigned int)WRITER_COUNT,
        (unsigned long)s_stats.count,
        (unsigned long)s_stats.errors,
        (unsigned long)avg_us,
        (unsigned long)max_us,
        (unsigned long)s_heap_before,
        (unsigned long)s_heap_after,
        (unsigned long)s_ram_added,
        (unsigned long)critical_us
    );

    if (n > 0 && n < (int)sizeof(report))
    {
        HAL_UART_Transmit(
            &huart1,
            (uint8_t *)report,
            (uint16_t)n,
            HAL_MAX_DELAY
        );
    }

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}