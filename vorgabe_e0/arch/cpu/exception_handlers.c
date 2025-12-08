#include <arch/cpu/exception_handlers.h>
#include <arch/cpu/exception_print.h>
#include <arch/bsp/irq_controller.h>
#include <arch/bsp/uart.h>
#include <arch/cpu/scheduler.h>
#include <stdbool.h>

extern bool irq_debug;

//the subtraction of 4 or 8 is necessary because of the way the exception trampolines are set up
void handle_supervisor_call_trampoline(register_context_t* ctx)
{
	if (ctx->r0 != 1) {
		print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr - 4, 0, 0, 0, 0);
	}
	// If in user mode (0b10000), kill the thread; otherwise hang
	if ((ctx->spsr & 0x1F) == 0x10) {
		scheduler_end_current_thread(ctx);
	} else {
		uart_putc('\4');
		while (true) {
		}
	}

}

void handle_undefined_instruction_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "Undefined Instruction", ctx->lr - 4, 0, 0, 0, 0);
	if ((ctx->spsr & 0x1F) == 0x10) {
		scheduler_end_current_thread(ctx);
	} else {
		uart_putc('\4');
		while (true) {
		}
	}
}

void handle_prefetch_abort_trampoline(register_context_t* ctx)
{
	unsigned int ifsr, ifar;
	asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r" (ifsr));
	asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r" (ifar));
	print_exception_infos(ctx, false, true, "Prefetch Abort", ctx->lr - 4, 0, 0, ifsr, ifar);
	if ((ctx->spsr & 0x1F) == 0x10) {
		scheduler_end_current_thread(ctx);
	} else {
		uart_putc('\4');
		while (true) {
		}
	}
}

void handle_data_abort_trampoline(register_context_t* ctx)
{
	unsigned int dfsr, dfar;
	asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (dfsr));
	asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (dfar));
	print_exception_infos(ctx, true, false, "Data Abort", ctx->lr - 8, dfsr, dfar, 0, 0);
	if ((ctx->spsr & 0x1F) == 0x10) {
		scheduler_end_current_thread(ctx);
	} else {
		uart_putc('\4');
		while (true) {
		}
	}
}

void handle_not_used_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "Not Used", ctx->lr, 0, 0, 0, 0);
	uart_putc('\4');
	while (true) {
	}
}

void handle_irq_trampoline(register_context_t* ctx)
{
	irq_controller_handler(ctx);
	if (irq_debug) {
		print_exception_infos(ctx, false, false, "IRQ", ctx->lr - 4, 0, 0, 0, 0);
	}
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "FIQ", ctx->lr - 4, 0, 0, 0, 0);
	uart_putc('\4');
	while (true) {
	}
}
