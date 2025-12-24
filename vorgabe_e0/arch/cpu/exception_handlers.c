#include <arch/cpu/exception_handlers.h>
#include <arch/cpu/exception_print.h>
#include <arch/bsp/irq_controller.h>
#include <arch/bsp/uart.h>
#include <arch/cpu/scheduler.h>
#include <arch/cpu/irq.h>
#include <kernel/syscalls.h>
#include <lib/kprintf.h>
#include <stdbool.h>

// Syscall dispatch table - function pointers indexed by syscall number
static void (*syscall_table[])(register_context_t *) = {
	[SYSCALL_EXIT] = syscall_handler_exit,
	[SYSCALL_PUTC] = syscall_handler_putc,
	[SYSCALL_GETC] = syscall_handler_getc,
	[SYSCALL_CREATE_THREAD] = syscall_handler_create_thread,
	[SYSCALL_SLEEP] = syscall_handler_sleep,
};

//the subtraction of 4 or 8 is necessary because of the way the exception trampolines are set up
void handle_supervisor_call_trampoline(register_context_t* ctx)
{
	// Check if called from kernel mode (must be user mode 0b10000)
	if ((ctx->spsr & 0x1F) != 0x10) {
		kprintf("Supervisor Call\n");
		print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr - 4, 0, 0, 0, 0);
		uart_putc('\4');
		while (true) {
		}
	}

	// Extract syscall number from r7
	unsigned int syscall_num = ctx->r7;

	// Check if syscall number is valid
	if (syscall_num > SYSCALL_MAX) {
		// Unknown syscall - terminate thread with register dump
		print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr - 4, 0, 0, 0, 0);
		scheduler_end_current_thread(ctx);
		return;
	}

	// Dispatch to syscall handler via function pointer table
	// Note: Interrupts are already masked in SVC mode
	// Critical sections within handlers should use irq_disable/restore if needed
	syscall_table[syscall_num](ctx);
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
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	print_exception_infos(ctx, false, false, "FIQ", ctx->lr - 4, 0, 0, 0, 0);
	uart_putc('\4');
	while (true) {
	}
}
