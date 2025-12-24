#include <arch/bsp/uart.h>
#include <arch/bsp/timer.h>
#include <arch/cpu/scheduler.h>
#include <lib/kprintf.h>
#include <config.h>
#include <stdbool.h>

void system_timer_init(void) {
	timer_instance->CS |= TIMER_STATUS_1;
	unsigned int counter = timer_instance->CLO;
	timer_instance->C1 = counter + TIMER_INTERVAL;
}

void system_timer_interrupt_handler(void) {
	unsigned int counter = timer_instance->CLO;
	unsigned int new_c1 = counter + TIMER_INTERVAL;

	timer_instance->C1 = new_c1;
	timer_instance->CS |= TIMER_STATUS_1;

	// Decrement sleep counters for sleeping threads
	scheduler_tick_sleep();

	kprintf("!");
}
