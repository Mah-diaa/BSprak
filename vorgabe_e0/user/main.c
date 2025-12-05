#include <user/main.h>
#include <lib/regcheck.h>
#include <config.h>
#include <arch/bsp/uart.h>
#include <arch/cpu/exception_triggers.h>

void main(void * args) {
	test_user(args);
	char c = *((char *) args);
	switch (c) {
		case 'a':
			do_data_abort();
			return;
		case 'p':
			do_prefetch_abort();
			return;
		case 'u':
			do_undefined_inst();
			return;
		case 's':
			do_supervisor_call();
			return;
		case 'c':
			register_checker();
			return;
	}
	
	for(unsigned int n = 0; n < PRINT_COUNT; n++){
		for(volatile unsigned int i = 0; i < BUSY_WAIT_COUNTER; i++){}
		uart_putc(c);
	}
}

