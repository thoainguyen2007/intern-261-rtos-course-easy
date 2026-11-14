/* Week 3 / Ex4 -- run-time stats on DWT CYCCNT.
 * Native FreeRTOS API only.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "runtime_stats.h"
#include "supervisor.h"


TaskHandle_t deadlock_mutex_holder_a(void);
TaskHandle_t deadlock_mutex_holder_b(void);


extern UART_HandleTypeDef huart1;

void runtime_stats_timer_init(void)
{
    /* TODO 1: same DWT enable sequence as week1/ex2 dwt_init() (CoreDebug
     *         DEMCR TRCENA bit, zero DWT->CYCCNT, DWT->CTRL CYCCNTENA bit).
     *         If you already have week1/ex2's dwt_init(), the sequence is
     *         identical -- do not invent a second mechanism. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t runtime_stats_timer_value(void)
{
    /* TODO 2: return DWT->CYCCNT. This is portGET_RUN_TIME_COUNTER_VALUE()'s
     *         implementation -- wire it up in FreeRTOSConfig.deltas. */
    return DWT->CYCCNT;
}


static void print_task_states(void)
{
    TaskStatus_t tasks[10];
    UBaseType_t count = uxTaskGetSystemState(tasks, 10, NULL);

    for (UBaseType_t i = 0; i < count; i++) {
        const char *state = "Other";

        if (tasks[i].eCurrentState == eRunning) state = "Running";
        else if (tasks[i].eCurrentState == eReady) state = "Ready";
        else if (tasks[i].eCurrentState == eBlocked) state = "Blocked";
        else if (tasks[i].eCurrentState == eSuspended) state = "Suspended";

        char line[80];
        int n = snprintf(line, sizeof(line),
                         "%s: %s\r\n",
                         tasks[i].pcTaskName, state);

        if (n > 0 && n < sizeof(line)) {
            HAL_UART_Transmit(&huart1, (uint8_t *)line, n, 100);
        }
    }
    TaskHandle_t owner_a = deadlock_mutex_holder_a();
TaskHandle_t owner_b = deadlock_mutex_holder_b();

const char *name_a = "NONE";
const char *name_b = "NONE";

for (UBaseType_t i = 0; i < count; i++) {
    if (owner_a != NULL && tasks[i].xHandle == owner_a)
        name_a = tasks[i].pcTaskName;

    if (owner_b != NULL && tasks[i].xHandle == owner_b)
        name_b = tasks[i].pcTaskName;
}

char msg[100];

int n = snprintf(msg, sizeof(msg),
                 "Mutex A held by: %s\r\n"
                 "Mutex B held by: %s\r\n",
                 name_a, name_b);

if (n > 0 && n < (int)sizeof(msg)) {
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)msg,
        (uint16_t)n,
        100
    );
}
}

void runtime_stats_task(void *argument)
{
    (void)argument;
    static char buf[512];

    for (;;) {
        /* TODO 3: vTaskGetRunTimeStats(buf) and print buf over UART.
         *         Note: this walks every task's TCB -- it is itself not free,
         *         factor that into why STATS_WINDOW_MS should not be too
         *         short. */
        vTaskGetRunTimeStats(buf);

        HAL_UART_Transmit(
            &huart1,
            (uint8_t *)buf,
            (uint16_t)strlen(buf),
            1000
        );

        const char newline[] = "\r\n";
        HAL_UART_Transmit(
            &huart1,
            (uint8_t *)newline,
            sizeof(newline) - 1,
            100
        );

        print_task_states();
        supervisor_checkin(SUPERVISOR_BIT_STATS);

        vTaskDelay(pdMS_TO_TICKS(STATS_WINDOW_MS));
    }
}

   
