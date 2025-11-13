#include <arch/bsp/timer.h>
#include <lib/kprintf.h>
#include <config.h>
#include <stdbool.h>

/* External flag to control IRQ debug output */
extern bool irq_debug;

void system_timer_init(void) {
	/* Clear timer interrupt status bit 1 (write 1 to clear) */
	timer_instance->CS |= TIMER_STATUS_1;
	
	/* Set compare register C1 to current time + interval */
	unsigned int counter = timer_instance->CLO;
	timer_instance->C1 = counter + TIMER_INTERVAL;
}

void system_timer_interrupt_handler(void) {
	/* 
	 * Timer Interrupt Flow:
	 * 1. Timer CLO counter reaches C1 compare value
	 * 2. Timer sets CS register bit 1 (interrupt pending)
	 * 3. IRQ Controller sees PENDING_1 bit 1 set
	 * 4. CPU receives IRQ, calls irq_controller_handler()
	 * 5. irq_controller_handler() calls this function
	 * 6. We must:
	 *    a) Set new C1 value for next interrupt (CLO + TIMER_INTERVAL)
	 *    b) Clear CS bit 1 to acknowledge interrupt
	 * 
	 * Important: Set C1 BEFORE clearing CS to avoid race condition where
	 * timer might fire again immediately if C1 is set too late.
	 */
	
	/* Update compare register for next interrupt FIRST */
	unsigned int counter = timer_instance->CLO;
	unsigned int new_c1 = counter + TIMER_INTERVAL;
	
	/* Set new C1 value BEFORE clearing interrupt (prevents immediate re-trigger) */
	timer_instance->C1 = new_c1;
	
	/* Memory barrier to ensure C1 write completes before clearing interrupt */
	__asm__ __volatile__("dmb sy" ::: "memory");
	
	/* Clear timer interrupt status bit 1 (write 1 to clear) */
	timer_instance->CS |= TIMER_STATUS_1;
	
	/* Print "!" on every timer interrupt as required by assignment */
	kprintf("!\n");
}

