#include <arch/bsp/uart.h>
#include <stddef.h>
#include <assert.h>


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


}


void uart_putc(char c) {
    while (uart_instance->FR & (1 << 5)) { } // Wait for TX not full
    uart_instance->DR = c; //this works because I cast the uart in the header, probably unsafe though.
}

char uart_getc(void) {
    while (uart_instance->FR & (1 << 4)) { }  // Wait for RX not empty
    return uart_instance->DR;
}


