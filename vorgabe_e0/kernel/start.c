#include <lib/kprintf.h>
#include <config.h>
#include <arch/bsp/uart.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/irq_controller.h>
#include <arch/cpu/exception_triggers.h>
#include <arch/cpu/scheduler.h>
#include <kernel/threads.h>
#include <user/main.h>
#include <lib/regcheck.h>
#include <stdbool.h>

bool irq_debug = false;

void start_kernel [[noreturn]] (void) {
	init_uart();
	irq_controller_init();
	system_timer_init();
	threads_init();
	scheduler_enable();
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();
	while(true) {
	}
}

