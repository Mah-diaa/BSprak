#include <arch/bsp/uart.h>
#include <lib/ringbuffer.h>
#include <config.h>
#include <stddef.h>
#include <assert.h>

/* Create ring buffer for UART RX characters */
create_ringbuffer(uart_rx_buffer, UART_INPUT_BUFFER_SIZE);


void init_uart(){
	//"Binding" to the GPIO
	gpio_port->func[1] = (gpio_port->func[1] & ~(GPF_MASK << TXD_SHIFT)) |
			     (ALT_FUNC_0 << TXD_SHIFT);

	gpio_port->func[1] = (gpio_port->func[1] & ~(GPF_MASK << RXD_SHIFT)) |
			     (ALT_FUNC_0 << RXD_SHIFT);
	//set the FIFO
	uart_instance->LCRH = 0;
	uart_instance->LCRH |= 1 << 4;//this enables the fifo
	uart_instance->LCRH |= 3 << 5;//this is word length? no clue what it does but setting to 8 for now.
	
	//set CR bits as per page 185
	
	uart_instance->CR = 0; //disable uart
	//i think disabling line ctrl is redundant
	
	uart_instance->CR |= 3 << 8; //enable txd and rxd
	uart_instance->CR |= 1 << 0;//enable uart
	
	/* Enable RX interrupt (bit 4 in IMSC) */
	uart_instance->IMSC = (1 << 4);
	
	/* Clear any pending RX interrupts */
	uart_instance->ICR = (1 << 4);
}


void uart_putc(char c) {
    while (uart_instance->FR & (1 << 5)) { } // Wait for TX not full
    uart_instance->DR = c; //this works because I cast the uart in the header, probably unsafe though.
}

char uart_getc(void) {
    /* Wait for data in ring buffer (interrupt-driven) */
    while (buff_is_empty(uart_rx_buffer)) { }
    return buff_getc(uart_rx_buffer);
}

/* Function to be called from IRQ handler when UART RX interrupt occurs */
void uart_rx_interrupt_handler(void) {
    /* Check if RX interrupt is pending (bit 4 in MIS) */
    if (uart_instance->MIS & (1 << 4)) {
        /* Read one character from UART data register (mask to 8 bits) and put in ring buffer */
        char c = uart_instance->DR & 0xFF;
        
        /* buff_putc returns true if buffer is full (error), false on success */
        if (buff_putc(uart_rx_buffer, c)) {
            /* Buffer is full - character is lost (could add error handling) */
        }
        
        /* Clear the RX interrupt by writing 1 to bit 4 in ICR */
        uart_instance->ICR |= (1 << 4);
    }
}

void uart_loopback [[noreturn]] (void) {
	while (true) {
		uart_putc(uart_getc());
	}
}

