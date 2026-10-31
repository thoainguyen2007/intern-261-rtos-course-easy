/* Week 2 / Ex3 — priority inversion.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "tasks_lmh.h"

void app_main(void)
{
    resource_init();
    dwt_init();

    /* TODO: xTaskCreate task_low, task_mid, task_high with three DISTINCT
     *       priorities, low < mid < high. Justify the exact numbers in
     *       REPORT.md — they must leave room for configMAX_PRIORITIES and not
     *       collide with any other task in the system. */

    xTaskCreate(task_low,"task_low", 512, NULL, 1, NULL);
    xTaskCreate(task_mid,"task_mid", 512, NULL, 2, NULL);
    xTaskCreate(task_high,"task_high", 512, NULL, 3, NULL);

    vTaskStartScheduler();

    for (;;) {
    }
}
