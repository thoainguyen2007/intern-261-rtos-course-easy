/* Week 3 / Ex3 -- UART RX interrupt -> stream buffer.
 * Native FreeRTOS API only.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "main.h"
#include "params.h"
#include "uart_rx.h"

StreamBufferHandle_t rx_stream;

extern UART_HandleTypeDef huart1;

static uint8_t s_rx_byte;
static volatile uint32_t s_rx_dropped = 0;
uint32_t uart_rx_get_dropped(void);

void uart_rx_init(void)
{
    /* TODO 1: rx_stream = xStreamBufferCreate(STREAM_BUFFER_BYTES,
     *         TRIGGER_LEVEL). Check non-NULL.
     *
     * TODO 2: HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1) to arm the first
     *         byte. huart1 must already be MX_USART1_UART_Init()'d at
     *         RX_BAUD -- that part lives in template/, not here. */
    rx_stream = xStreamBufferCreate(STREAM_BUFFER_BYTES, TRIGGER_LEVEL);
    configASSERT(rx_stream != NULL);

    HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1);

}

void uart_rx_isr_handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* TODO 3: xStreamBufferSendFromISR(rx_stream, &s_rx_byte, 1,
     *         &xHigherPriorityTaskWoken). Check the return value: what do you
     *         do with the byte if the stream buffer is full and it returns 0?
     *         Answer this in PREDICTION.md, not just here. */
    size_t sent =xStreamBufferSendFromISR(rx_stream, &s_rx_byte, 1,&xHigherPriorityTaskWoken);
    if (sent == 0){
        s_rx_dropped++;
    }

    /* TODO 4: HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1) again -- re-arm for
     *         the next byte. Forgetting this is the single most common way
     *         to silently stop receiving after the first byte. */

    HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1);

    /* TODO 5: portYIELD_FROM_ISR(xHigherPriorityTaskWoken). */

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    (void)xHigherPriorityTaskWoken;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uart_rx_isr_handler();
    }
}

uint32_t uart_rx_get_dropped(void)
{
    return s_rx_dropped;
}