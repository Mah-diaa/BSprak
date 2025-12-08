#include <arch/bsp/uart.h>
#include <lib/ringbuffer.h>
#include <config.h>
#include <stddef.h>
#include <assert.h>
#include <arch/cpu/exception_triggers.h>
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

	// Clear RX and RX timeout interrupts before enabling as per docs
	uart_instance->ICR = (1 << 4) | (1 << 6);
	uart_instance->IMSC = (1 << 4) | (1 << 6);

	// drain any characters that arrived early may help with fast incoming chars 
	while (!(uart_instance->FR & (1 << 4))) {  // While RX FIFO not empty
		char c = uart_instance->DR & 0xFF;
		buff_putc(uart_buffer, c);
	}

	uart_instance->ICR = (1 << 4) | (1 << 6);
}

void uart_putc(char c) {
    while (uart_instance->FR & (1 << 5)) { }
    uart_instance->DR = c;
}

char uart_getc(void) {
    while (buff_is_empty(uart_buffer)) { }
    return buff_getc(uart_buffer);
}

extern bool irq_debug;

//adding the handling for the Kernel crashing here
void uart_rx_interrupt_handler(void) {
    // Check for RX interrupt
    if (uart_instance->MIS & ((1 << 4) | (1 << 6))) {
        while (!(uart_instance->FR & (1 << 4))) {  // Read ALL characters from FIFO
            char c = uart_instance->DR & 0xFF;
            switch(c) {
                case 'd':
                    irq_debug = !irq_debug;
                    break;
                case 'S':
                    do_supervisor_call();
                    break;
                case 'P':
                    do_prefetch_abort();
                    break;
                case 'A':
                    do_data_abort();
                    break;
                case 'U':
                    do_undefined_inst();
                    break;
               default:
                    // Create thread for all other characters
                    scheduler_thread_create(main, &c, sizeof(char));
                    break;
            }
        }
        uart_instance->ICR = (1 << 4) | (1 << 6);
    }
}

void uart_loopback [[noreturn]] (void) {
	while (true) {
		uart_putc(uart_getc());
	}
}
