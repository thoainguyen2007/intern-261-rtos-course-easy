/* Week 1 / Ex2 — priorities, preemption, starvation.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "tasks.h"

void app_main(void)
{
    dwt_init();

    /* TODO: create the three tasks below with the priorities from params.h.
     *       Keep the stacks as small as you can still justify — ex4 will make
     *       you prove the number. */
    xTaskCreate(task_a, "TaskA", configMINIMAL_STACK_SIZE, NULL, TASK_A_PRIO, NULL);
    xTaskCreate(task_b, "TaskB", configMINIMAL_STACK_SIZE, NULL, TASK_B_PRIO, NULL);
    xTaskCreate(task_c, "TaskC", configMINIMAL_STACK_SIZE, NULL, TASK_C_PRIO, NULL);

    vTaskStartScheduler();

    for (;;) {
    }
}
