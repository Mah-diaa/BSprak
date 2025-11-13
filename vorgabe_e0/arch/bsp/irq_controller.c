#include <arch/bsp/irq_controller.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/uart.h>

void irq_controller_init(void) {
	/* Enable timer interrupt in ENABLE_1 register */
	irq_controller_instance->ENABLE_1 |= TIMER_INTERRUPT_MASK;
	
	/* Enable UART interrupt in ENABLE_2 register */
	irq_controller_instance->ENABLE_2 |= UART_INTERRUPT_MASK;
}

void irq_controller_handler(void) {
	/* 
	 * Interrupt Flow:
	 * 1. Hardware peripheral (Timer/UART) sets its interrupt flag
	 * 2. IRQ Controller sets corresponding bit in PENDING_1 or PENDING_2
	 * 3. CPU receives IRQ exception and calls this handler
	 * 4. We check which interrupt is pending and call the appropriate handler
	 * 5. The peripheral handler clears the interrupt flag in the peripheral
	 * 6. PENDING registers clear automatically when source interrupt is cleared
	 * 
	 * Note: PENDING_1/PENDING_2 are read-only status registers.
	 * They clear automatically when the source interrupt flag is cleared.
	 */
	
	/* Check if timer interrupt is pending (bit 1 in PENDING_1) */
	if (irq_controller_instance->PENDING_1 & TIMER_INTERRUPT_MASK) {
		system_timer_interrupt_handler();
	}
	
	/* Check if UART interrupt is pending (bit 25 in PENDING_2) */
	if (irq_controller_instance->PENDING_2 & UART_INTERRUPT_MASK) {
		uart_rx_interrupt_handler();
	}
}

