/* Week 3 / Ex1 -- ISR -> task wakeup, NVIC priority.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "exti_button.h"

void app_main(void)
{
    exti_button_init();

    /* TODO: xTaskCreate ack_task. Keep its priority high enough that it is
     *       the thing that actually determines your measured latency, not
     *       scheduling jitter from something else. Justify the number in
     *       REPORT.md. */

    xTaskCreate(ack_task, "ack_task", 128, NULL, 4, NULL);//ko liên quan tới mấy cái priority của NVIC, đặt 4 là do configMAX_PRIORITIES=5 nên có 5 mức priority cho các task(chọn con số từ 0-4 là đc)

   

    vTaskStartScheduler();

    for (;;) {
    }
}
