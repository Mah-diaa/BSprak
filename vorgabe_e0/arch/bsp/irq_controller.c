#include <arch/bsp/irq_controller.h>
#include <arch/bsp/timer.h>
#include <arch/bsp/uart.h>
#include <arch/cpu/registers.h>
#include <arch/cpu/scheduler.h>

void irq_controller_init(void) {
	irq_controller_instance->ENABLE_1 |= TIMER_INTERRUPT_MASK;
	irq_controller_instance->ENABLE_2 |= UART_INTERRUPT_MASK;
}

void irq_controller_handler(register_context_t *ctx) {
	if (irq_controller_instance->PENDING_2 & UART_INTERRUPT_MASK) {
		uart_rx_interrupt_handler();
	}
	if (irq_controller_instance->PENDING_1 & TIMER_INTERRUPT_MASK) {
		system_timer_interrupt_handler();
		scheduler_schedule(ctx);
	}
}
