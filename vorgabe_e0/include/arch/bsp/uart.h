#ifndef UART_H
#define UART_H

#include "arch/bsp/yellow_led.h"
#include <stddef.h>
#include <assert.h>

#define UART_RX_INTERRUPT (1 << 4)
#define UART_RT_INTERRUPT (1 << 6)
#define UART_RX_RT_INTERRUPTS (UART_RX_INTERRUPT | UART_RT_INTERRUPT)

#define UART_FR_RXFE (1 << 4)
#define UART_FR_TXFF (1 << 5)

struct uart {
	unsigned int DR;
	unsigned int RSRECR;
	unsigned int unused1[4]; //0x8 0xC 0x10 0x14 unused
	unsigned int FR;
	unsigned int unused2[2]; // 0x1C 0x20 unused
	unsigned int IBRD;
	unsigned int FBRD;
	unsigned int LCRH;
	unsigned int CR;
	unsigned int IFLS;
	unsigned int IMSC;
	unsigned int RIS;
	unsigned int MIS;
	unsigned int ICR;
};

static_assert(offsetof(struct uart, DR) == 0x00);
static_assert(offsetof(struct uart, RSRECR) == 0x4);
static_assert(offsetof(struct uart, FR) == 0x18);
static_assert(offsetof(struct uart, IBRD) == 0x24);
static_assert(offsetof(struct uart, FBRD) == 0x28);
static_assert(offsetof(struct uart, LCRH) == 0x2c);
static_assert(offsetof(struct uart, CR) == 0x30);
static_assert(offsetof(struct uart, IFLS) == 0x34);
static_assert(offsetof(struct uart, IMSC) == 0x38);
static_assert(offsetof(struct uart, RIS) == 0x3C);
static_assert(offsetof(struct uart, MIS) == 0x40);
static_assert(offsetof(struct uart, ICR) == 0x44);

void init_uart();
void uart_putc(char c);
char uart_getc();
bool uart_has_char(void);
bool uart_try_getc(char *out);
void uart_rx_interrupt_handler(void);
void uart_loopback [[noreturn]] (void);

#endif
