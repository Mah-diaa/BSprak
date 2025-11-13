

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
	unsigned int unused2; // 0x34
	unsigned int IMSC;  /* Interrupt Mask Set/Clear at offset 0x38 */
	unsigned int RIS;   /* Raw Interrupt Status at offset 0x3C */
	unsigned int MIS;   /* Masked Interrupt Status at offset 0x40 */
	unsigned int ICR;   /* Interrupt Clear Register at offset 0x44 */
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


static volatile struct uart *const uart_instance = (struct uart *)UART_BASE;


void init_uart();
void uart_putc(char c);
char uart_getc();
void uart_rx_interrupt_handler(void);  /* Call this from IRQ handler when UART RX interrupt occurs */
void uart_loopback [[noreturn]] (void);




