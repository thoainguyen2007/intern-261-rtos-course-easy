/* Week 1 / Ex4 — FreeRTOS hooks.
 *
 * Both hooks must be observable from outside the chip. A breakpoint is not a
 * deliverable: you need a UART line or a distinct LED pattern you can photograph.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "params.h"


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* TODO 1: report xTask / pcTaskName out of the chip, then stop.
     *
     * Careful: the stack of the offending task is already corrupt. Think about
     * what you are allowed to call here, and write the reasoning in REPORT.md.
     * Hint: this hook runs from the scheduler's context, not the task's.
     */
    (void)xTask;

    /* 1. Gửi chuỗi cảnh báo trực tiếp bằng thanh ghi UART (không phụ thuộc FreeRTOS/ngắt) */
    const char msg[] = "\r\n[HOOK] PHAT HIEN TRAN STACK: ";
    for (const char *p = msg; *p != '\0'; p++) {
        while (!(USART1->SR & USART_SR_TXE)); /* Đợi bộ đệm truyền rỗng */
        USART1->DR = (uint8_t)(*p);
    }

    /* In tiếp tên task bị tràn nếu có */
    if (pcTaskName != NULL) {
        for (const char *p = pcTaskName; *p != '\0'; p++) {
            while (!(USART1->SR & USART_SR_TXE));
            USART1->DR = (uint8_t)(*p);
        }
    }

    const char nl[] = "\r\n";
    for (const char *p = nl; *p != '\0'; p++) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = (uint8_t)(*p);
    }
    while (!(USART1->SR & USART_SR_TC)); /* Đợi truyền xong ký tự cuối */

    /* 2. Nháy LED PC13 thật nhanh và dừng chip tại đây */
    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (volatile int i = 0; i < 200000; i++);
    }
}

void vApplicationMallocFailedHook(void)
{
    /* TODO 2: report, then stop. */
    /* 1. Gửi thông báo hết Heap trực tiếp */
    const char msg[] = "\r\n[HOOK] MALLOC THAT BAI: FreeRTOS het Heap!\r\n";
    for (const char *p = msg; *p != '\0'; p++) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = (uint8_t)(*p);
    }
    while (!(USART1->SR & USART_SR_TC));

    /* 2. Nháy LED PC13 chậm hơn và dừng chip */
    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (volatile int i = 0; i < 1000000; i++);
    }
}

/* configSUPPORT_STATIC_ALLOCATION = 1 makes these two callbacks mandatory.
 * Provide the memory for the idle task and the timer task here. */

 /* 1. Chuẩn bị sẵn TCB và mảng Stack cho Idle Task */
static StaticTask_t xIdleTaskTCB;
static StackType_t  uxIdleTaskStack[configMINIMAL_STACK_SIZE]; /* Mảng Stack (đơn vị: Words) */

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* TODO 3: static buffers. Note the size is in WORDS, not bytes —
     *         getting this wrong is a silent overflow. */
/* Đưa địa chỉ TCB và Stack cho FreeRTOS dùng */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE; // Độ sâu stack tính bằng Words
}


/* 2. Chuẩn bị sẵn TCB và mảng Stack cho Timer Task */
static StaticTask_t xTimerTaskTCB;
static StackType_t  uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    /* TODO 4: same, for the software timer task. */
/* Đưa địa chỉ TCB và Stack của Timer cho FreeRTOS */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH; // Độ sâu stack tính bằng Words
}
