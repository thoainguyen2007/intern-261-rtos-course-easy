#ifndef UART_GUARD_H
#define UART_GUARD_H

#include <stddef.h>

/* 0 = none (the bug), 1 = mutex, 2 = critical section, 3 = gatekeeper task.
 * Build this file four times over the exercise, once per mode. */
#ifndef UART_GUARD_MODE
#define UART_GUARD_MODE 3
#endif

void uart_guard_init(void);

/* Blocking: sends `len` bytes out USART1 under whatever UART_GUARD_MODE picks.
 * In gatekeeper mode this enqueues to the gatekeeper instead of transmitting
 * directly — same call site either way. */
void uart_guard_send(const char *buf, size_t len);

#if UART_GUARD_MODE == 3
void gatekeeper_task(void *argument);
#endif


//Latency
#define MEASURE_TOTAL_MESSAGES 100U

void latency_report_task(void *argument);

#endif /* UART_GUARD_H */