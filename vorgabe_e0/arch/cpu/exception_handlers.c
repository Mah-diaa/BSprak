#include <arch/cpu/exception_handlers.h>
#include <arch/cpu/exception_print.h>
#include <arch/bsp/irq_controller.h>
#include <arch/bsp/uart.h>
#include <stdbool.h>

extern bool irq_debug;

void handle_supervisor_call_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr);
	while (true) {
		uart_putc('\4');
	}
}

void handle_undefined_instruction_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "Undefined Instruction", ctx->lr);
	while (true) {
		uart_putc('\4');
	}
}

void handle_prefetch_abort_trampoline(register_context_t* ctx)
{
	asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r" (ctx->ifsr));
	asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r" (ctx->ifar));
	print_exception_infos(ctx, false, true, "Prefetch Abort", ctx->ifar);
	while (true) {
		uart_putc('\4');
	}
}

void handle_data_abort_trampoline(register_context_t* ctx)
{
	asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (ctx->dfsr));
	asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (ctx->dfar));
	print_exception_infos(ctx, true, false, "Data Abort", ctx->lr);
	while (true) {
		uart_putc('\4');
	}
}

void handle_not_used_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "Not Used", ctx->lr);
	while (true) {
		uart_putc('\4');
	}
}

void handle_irq_trampoline(register_context_t* ctx)
{
	irq_controller_handler();
	if (irq_debug) {
		print_exception_infos(ctx, false, false, "IRQ", ctx->lr);
	}
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "FIQ", ctx->lr);
	while (true) {
		uart_putc('\4');
	}
}
