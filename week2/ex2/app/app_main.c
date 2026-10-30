/* Week 2 / Ex2 — race condition & mutex.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "uart_guard.h"
#include <stdint.h>

void writer_task(void *argument);

void app_main(void)
{

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


    uart_guard_init();

    /* TODO: xTaskCreate WRITER_COUNT instances of writer_task, each given its
     *       own index (0..WRITER_COUNT-1) as the task parameter.
     *       Do not create WRITER_COUNT separately-named functions. */
    for (uint32_t i = 0; i < WRITER_COUNT; i++){
        BaseType_t result = xTaskCreate(
            writer_task,
            "Writer",
            216,
            (void *)(uintptr_t)i,
            2,
            NULL
        );

        configASSERT(result == pdPASS);
    }



    
    BaseType_t result = xTaskCreate(
        latency_report_task,
        "Report",
        384,
        NULL,
        1,
        NULL
    );

    configASSERT(result == pdPASS);




    vTaskStartScheduler();

    for (;;) {
    }
}