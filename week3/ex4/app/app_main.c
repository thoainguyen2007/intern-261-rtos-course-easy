/* Week 3 / Ex4 -- CPU load, deadlock, watchdog.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "runtime_stats.h"
#include "deadlock_demo.h"
#include "supervisor.h"

extern UART_HandleTypeDef huart1;


void app_main(void)
{
    const char boot[] = "\r\n=== BOOT ===\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)boot, sizeof(boot)-1, 100);
    runtime_stats_timer_init();
    deadlock_demo_init();
    supervisor_init();

    /* TODO: xTaskCreate task_alpha, task_beta, supervisor_task,
     *       runtime_stats_task. Run deadlock_demo exactly as given first
     *       (step 2 of README) before touching task_alpha/task_beta. */

    xTaskCreate(task_alpha, "Alpha", 192, NULL, 2, NULL);
    xTaskCreate(task_beta, "Beta", 192, NULL, 2, NULL);
    xTaskCreate(runtime_stats_task, "Stats", 384, NULL, 1, NULL);
    xTaskCreate(supervisor_task, "Supervisor", 256, NULL, 3, NULL);

    vTaskStartScheduler();

    for (;;) {
    }
}
