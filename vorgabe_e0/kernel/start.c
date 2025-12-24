#include <lib/kprintf.h>
#include <config.h>
#include <arch/bsp/uart.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/irq_controller.h>
#include <arch/cpu/exception_triggers.h>
#include <arch/cpu/scheduler.h>
#include <arch/cpu/irq.h>
#include <kernel/threads.h>
#include <user/main.h>
#include <lib/regcheck.h>
#include <stdbool.h>

bool irq_debug = false;

void start_kernel [[noreturn]] (void) {
	// Initialize hardware
	init_uart();
	irq_controller_init();
	system_timer_init();

	// Initialize threading system
	threads_init();

	// Print boot message
	kprintf("=== Betriebssystem gestartet ===\n");

	// Run kernel tests (if defined)
	test_kernel();

	// Create initial user-space thread running main()
	scheduler_thread_create(main, NULL, 0);

	// Enable scheduler
	scheduler_enable();

	// Enable interrupts so timer can trigger scheduling
	irq_enable();

	// Idle loop - wait for interrupts to schedule threads
	while(true) {
		// Just loop - interrupts will fire
	}
}

