#include <arch/bsp/uart.h>
#include <lib/ringbuffer.h>
#include <lib/kprintf.h>
#include <lib/string.h>
#include <config.h>
#include <stddef.h>
#include <assert.h>
#include <arch/cpu/exception_triggers.h>
#include <arch/cpu/exception_print.h>
#include <arch/cpu/registers.h>
#include <arch/cpu/scheduler.h>
#include <kernel/threads.h>
#include <user/main.h>
#include <stdbool.h>

static const unsigned int UART_BASE = 0x7E201000 - 0x3F000000;
static const unsigned int TXD = 14u % 10;
static const unsigned int RXD = 15u % 10;
static const unsigned int TXD_SHIFT = TXD * GPF_BITS;
static const unsigned int RXD_SHIFT = RXD * GPF_BITS;
static const unsigned int ALT_FUNC_0 = 0x4;

static volatile struct uart *const uart_instance = (struct uart *)UART_BASE;

create_ringbuffer(uart_buffer, UART_INPUT_BUFFER_SIZE);

void init_uart(){
	gpio_port->func[1] = (gpio_port->func[1] & ~(GPF_MASK << TXD_SHIFT)) |
			     (ALT_FUNC_0 << TXD_SHIFT);

	gpio_port->func[1] = (gpio_port->func[1] & ~(GPF_MASK << RXD_SHIFT)) |
			     (ALT_FUNC_0 << RXD_SHIFT);
	
	//set the FIFO
	uart_instance->LCRH = 0;
	uart_instance->LCRH |= 1 << 4; // Enable FIFO (FEN)
	uart_instance->LCRH |= 3 << 5; // 8-bit word length (WLEN)

	
	// this effectivly sets the trigger level to 1/8 full since 000 in bits 5:3
	uart_instance->IFLS = 0;
	
	//set CR bits as per page 185
	uart_instance->CR = 0; //disable uart
	uart_instance->CR |= 3 << 8; //enable txd and rxd
	uart_instance->CR |= 1 << 0;//enable uart

	uart_instance->ICR = UART_RX_RT_INTERRUPTS;
	uart_instance->IMSC = UART_RX_RT_INTERRUPTS;

	while (!(uart_instance->FR & (1 << 4))) {  // While RX FIFO not empty
		char c = uart_instance->DR & 0xFF;
		buff_putc(uart_buffer, c);
	}

	uart_instance->ICR = UART_RX_RT_INTERRUPTS;
}

void uart_putc(char c) {
    while (uart_instance->FR & (1 << 5)) { }
    uart_instance->DR = c;
}

char uart_getc(void) {
    while (buff_is_empty(uart_buffer)) { }
    return buff_getc(uart_buffer);
}

bool uart_has_char(void) {
    return !buff_is_empty(uart_buffer);
}

bool uart_try_getc(char *out) {
    if (buff_is_empty(uart_buffer)) {
        return false;
    }
    *out = buff_getc(uart_buffer);
    return true;
}

void uart_rx_interrupt_handler(void) {
    if (uart_instance->MIS & UART_RX_RT_INTERRUPTS) {
        uart_instance->ICR = UART_RX_RT_INTERRUPTS;
        while (!(uart_instance->FR & (1 << 4))) {
            char c = uart_instance->DR & 0xFF;

            buff_putc(uart_buffer, c);

            scheduler_wake_waiting_threads();
        }
    }
}

void uart_loopback [[noreturn]] (void) {
	while (true) {
		uart_putc(uart_getc());
	}
}
