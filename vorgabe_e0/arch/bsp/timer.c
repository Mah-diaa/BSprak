#include <arch/bsp/timer.h>
#include <lib/kprintf.h>
#include <config.h>
#include <stdbool.h>

/* External flag to control IRQ debug output */
extern bool irq_debug;

void system_timer_init(void) {
	/* Clear timer interrupt status bit 1 */
	timer_instance->CS |= TIMER_STATUS_1;
	
	/* Set compare register C1 to current time + interval */
	unsigned int counter = timer_instance->CLO;
	timer_instance->C1 = counter + TIMER_INTERVAL;
}

void system_timer_interrupt_handler(void) {
	/* Update compare register for next interrupt */
	unsigned int counter = timer_instance->CLO;
	timer_instance->C1 = counter + TIMER_INTERVAL;
	
	/* Clear timer interrupt status bit 1 (write 1 to clear) */
	timer_instance->CS |= TIMER_STATUS_1;
	
	/* Debug output */
	kprintf("!\n");
}

