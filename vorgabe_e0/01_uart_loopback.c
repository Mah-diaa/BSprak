void uart_loopback [[noreturn]] (void){
	while(true){
		uart_putc(uart_getc());
	}
}