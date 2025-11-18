#ifndef UART_H
#define UART_H

#include "arch/bsp/yellow_led.h"
#include <stddef.h>
#include <assert.h>

struct uart {
	unsigned int DR;
	unsigned int RSRECR;
	unsigned int unused1[4]; //0x8 0xC 0x10 0x14 unused
	unsigned int FR;
	unsigned int unused2[4];
	unsigned int LCRH;
	unsigned int CR;
	unsigned int unused3;
	unsigned int IMSC;
	unsigned int RIS;
	unsigned int MIS;
	unsigned int ICR;
};

static_assert(offsetof(struct uart, DR) == 0x00);
static_assert(offsetof(struct uart, RSRECR) == 0x4);
static_assert(offsetof(struct uart, FR) == 0x18);
static_assert(offsetof(struct uart, LCRH) == 0x2c);
static_assert(offsetof(struct uart, CR) == 0x30);
static_assert(offsetof(struct uart, IMSC) == 0x38);
static_assert(offsetof(struct uart, RIS) == 0x3C);
static_assert(offsetof(struct uart, MIS) == 0x40);
static_assert(offsetof(struct uart, ICR) == 0x44);

void init_uart();
void uart_putc(char c);
char uart_getc();
void uart_rx_interrupt_handler(void);  /* Call this from IRQ handler when UART RX interrupt occurs */
void uart_loopback [[noreturn]] (void);

#endif
