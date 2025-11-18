#include <arch/bsp/irq_controller.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/uart.h>

void irq_controller_init(void) {
	// Enable timer interrupt
	irq_controller_instance->ENABLE_1 |= TIMER_INTERRUPT_MASK;
	// Enable UART interrupt
	irq_controller_instance->ENABLE_2 |= UART_INTERRUPT_MASK;
}

void irq_controller_handler(void) {
	if (irq_controller_instance->PENDING_1 & TIMER_INTERRUPT_MASK) {
		system_timer_interrupt_handler();
	}
	if (irq_controller_instance->PENDING_2 & UART_INTERRUPT_MASK) {
		uart_rx_interrupt_handler();
	}
}