#include <lib/kprintf.h>
#include <config.h>
#include <arch/bsp/uart.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/irq_controller.h>
#include <arch/cpu/exception_triggers.h>
#include <lib/regcheck.h>
#include <stdbool.h>

static void subprogram [[noreturn]] (void);

bool irq_debug = false;

void start_kernel [[noreturn]] (void) {
	init_uart();
	irq_controller_init();
	system_timer_init();
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();

	while(true) {
		char c = uart_getc();
		uart_putc(c);
		switch(c) {
			case 'd':
				irq_debug = !irq_debug;
				break;
			case 'a':
				do_data_abort();
				break;
			case 'p':
				do_prefetch_abort();
				break;
			case 's':
				do_supervisor_call();
				break;
			case 'u':
				do_undefined_inst();
				break;
			case 'c':
				register_checker();
				break;
			case 'e':
				subprogram();
			default:
				kprintf("Unknown input: %c\n", c);
				break;
		}
	}
}

static void subprogram [[noreturn]] (void) {
	while(true) {
		char c = uart_getc();
		for(unsigned int n = 0; n < PRINT_COUNT; n++) {
			uart_putc(c);
			volatile unsigned int i = 0;
			for(; i < BUSY_WAIT_COUNTER; i++) {}
		}
	}
}
