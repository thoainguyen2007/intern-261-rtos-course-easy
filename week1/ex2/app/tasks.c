/* Week 1 / Ex2 — the three competing tasks.
 *
 * Each task: raise its pin, burn exactly TASK_x_WORK_US of CPU, lower its pin,
 * and loop again WITHOUT blocking. The missing block is the point of the
 * exercise, not a bug to fix in step 2.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"
#include "tasks.h"

/* TODO: one helper that takes a port/pin and a work time, then three thin
 *       wrappers. Do not copy-paste the body three times. */
static void task_loop(GPIO_TypeDef *port, uint16_t pin_mask, uint32_t work_us)
{
    /* BSRR (Bit Set/Reset Register): 16 bit thấp dùng để KÉO CHÂN LÊN MỨC 1 */
    port->BSRR = pin_mask;

    /* Bắt CPU quay cuồng đốt đúng số micro-giây quy định mà không nhả quyền */
    dwt_busy_wait_us(work_us);

    /* BRR (Bit Reset Register): KÉO CHÂN XUỐNG MỨC 0 */
    port->BRR = pin_mask;
}
void task_a(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO */
        (void)argument;
        task_loop(GPIOA, (1U << TASK_A_PIN), TASK_A_WORK_US);
        //vTaskDelay(1);
    }
}

void task_b(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO */
        task_loop(GPIOA, (1U << TASK_B_PIN), TASK_B_WORK_US);
        //vTaskDelay(1);

    }
}

void task_c(void *argument)
{
    (void)argument;
    for (;;) {
        /* TODO */
        task_loop(GPIOA, (1U << TASK_C_PIN), TASK_C_WORK_US*3);
        //vTaskDelay(1);
    }
}
