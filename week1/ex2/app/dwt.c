/* DWT cycle counter — the only honest way to burn a known number of
 * microseconds of CPU inside a task without blocking.
 */
#include "stm32f1xx.h"
#include "tasks.h"
#include "FreeRTOSConfig.h"
#include <stdint.h>

void dwt_init(void)
{
    /* TODO 1: enable the trace subsystem (CoreDebug DEMCR, TRCENA bit),
     *         zero DWT->CYCCNT, then enable CYCCNTENA.
     *         PM0056 does not document DWT; see the ARMv7-M ARM, C1.8. */
    /* Bật hệ thống con Trace (enable the trace subsystem (CoreDebug DEMCR, TRCENA bit)) */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    /* Reset bộ đếm chu kỳ về 0(zero DWT->CYCCNT) */
    DWT->CYCCNT = 0;
    /* Bật bộ đếm chu kỳ (enable CYCCNTENA) */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void dwt_busy_wait_us(uint32_t microseconds)
{
    /* TODO 2: spin until `microseconds` worth of CPU cycles have elapsed.
     *         SystemCoreClock is 72000000. Handle CYCCNT wrap-around correctly
     *         — unsigned subtraction, not a comparison of absolute values.
     *         Explain in REPORT.md why the naive comparison breaks every ~59 s. */

    /* Đổi thời gian micro-giây sang số chu kỳ CPU:
     * configCPU_CLOCK_HZ = 72000000 -> (72000000 / 1000000) = 72 chu kỳ cho mỗi 1 us
     -->cycle =microseconde*72, tức là số chu kỳ CPU trong thời gian busy.
     */
    uint32_t cycles = microseconds * (72UL);
    
    /* Ghi nhận mốc chu kỳ bắt đầu */
    uint32_t start = DWT->CYCCNT;

    /* Quay vòng lặp bận cho đến khi đủ số chu kỳ:
     * Dùng phép trừ không dấu uint32_t: (current - start)
     * Đây là điểm cốt lõi để chống lỗi tràn số (wrap-around).
     * CYCCNT là thanh ghi 32 bit nên sẽ đếm tới 2^32-1. Khi hết thì sẽ quay lại.
     * Để dễ hiểu thì xét trường hợp CYCCNT đếm tới 100 đi. Vd start là 90, cần busy trong 20 chu kỳ
     * Vậy thì khi hết 20 chu kỳ CYCCNT đang ở 10. 
     * Tuy nhiên, khi quay lại đếm từ 0 thì sẽ xuất hiện phép trừ 0-90=-90. 
     * Như vậy khi này sẽ ra âm và hiển nhiên <cycles và thoát vòng lặp luôn.
     * Để khắc phục cái này thì sẽ đổi thành uint32_t, khi này -90 =10 (modulo 100).
     *  */
    while ((uint32_t)(DWT->CYCCNT - start) < cycles);
}
