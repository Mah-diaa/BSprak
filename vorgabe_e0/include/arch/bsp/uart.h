

#include "arch/bsp/yellow_led.h"
#include <stddef.h>
#include <assert.h>


static const unsigned int UART_BASE =  0x7E201000 - 0x3F000000;
static const unsigned int TXD=  14u % 10;
static const unsigned int RXD =  15u % 10;
static const unsigned int TXD_SHIFT =  TXD * GPF_BITS;
static const unsigned int RXD_SHIFT=  RXD * GPF_BITS;
static const unsigned int ALT_FUNC_0=  0x4;



struct uart {
	unsigned int DR;
	unsigned int RSRECR;
	unsigned int unused[4]; //0x8 0xC 0x10 0x14 unused
	unsigned int FR;
	unsigned int unsused[4];
	unsigned int LCRH;
	unsigned int CR;
};



static_assert(offsetof(struct uart, DR) == 0x00);
static_assert(offsetof(struct uart, RSRECR) == 0x4);
static_assert(offsetof(struct uart, FR) == 0x18);
static_assert(offsetof(struct uart, LCRH) == 0x2c);
static_assert(offsetof(struct uart, CR) == 0x30);


static volatile struct uart *const uart_instance = (struct uart *)UART_BASE;


void init_uart();
void uart_putc(char c);
char uart_getc();
void uart_loopback [[noreturn]] (void);




