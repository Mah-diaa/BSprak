#include <arch/cpu/exception_handlers.h>
#include <arch/cpu/exception_print.h>
#include <arch/bsp/irq_controller.h>
#include <lib/kprintf.h>
#include <stddef.h>
#include <stdbool.h>

/* External flag to control IRQ debug output */
extern bool irq_debug;

/* Exception return offsets */
#define IRQ_RETURN_OFFSET 8

/* Stack frame layout: r0-r12 (13 regs), then lr */
#define REGS_SAVED_BEFORE_LR 13
#define LR_STACK_OFFSET (REGS_SAVED_BEFORE_LR * sizeof(unsigned int))

/* Exception handler implementations
 * TODO: Implement these functions to handle each exception type
 */

void handle_supervisor_call_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* The trampoline adjusted LR before saving (LR = instruction address), so ctx->lr contains the instruction address */
	print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_undefined_instruction_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* The trampoline adjusted LR before saving (LR = instruction address), so ctx->lr contains the instruction address */
	print_exception_infos(ctx, false, false, "Undefined Instruction", ctx->lr);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_prefetch_abort_trampoline(register_context_t* ctx)
{
	/* Read IFSR and IFAR from CP15 */
	asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r" (ctx->ifsr));  /* IFSR: c5, c0, 1 */
	asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r" (ctx->ifar));  /* IFAR: c6, c0, 2 */
	
	/* Print exception information */
	/* For Prefetch Abort, use IFAR as the exception source address (the address that couldn't be fetched) */
	print_exception_infos(ctx, false, true, "Prefetch Abort", ctx->ifar);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_data_abort_trampoline(register_context_t* ctx)
{
	/* Read DFSR and DFAR from CP15 */
	asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (ctx->dfsr));  /* DFSR: c5, c0, 0 */
	asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (ctx->dfar));  /* DFAR: c6, c0, 0 */
	
	/* Print exception information */
	/* The trampoline adjusted LR before saving (LR = instruction address), so ctx->lr contains the instruction address */
	print_exception_infos(ctx, true, false, "Data Abort", ctx->lr);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_not_used_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* The trampoline didn't adjust LR (return_offset = 0), so ctx->lr contains the return address */
	print_exception_infos(ctx, false, false, "Not Used", ctx->lr);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_irq_trampoline(register_context_t* ctx)
{
	/* Handle interrupt sources - check IRQ controller to see which interrupt fired */
	irq_controller_handler();
	
	/* Print exception information if debug is enabled */
	/* The trampoline adjusted LR before saving (LR = PC), so ctx->lr contains PC */
	if (irq_debug) {
		print_exception_infos(ctx, false, false, "IRQ", ctx->lr);
	}
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* The trampoline adjusted LR before saving (LR = instruction address), so ctx->lr contains the instruction address */
	print_exception_infos(ctx, false, false, "FIQ", ctx->lr);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

