#include <arch/cpu/exception_handlers.h>
#include <arch/cpu/exception_print.h>
#include <lib/kprintf.h>

/* Exception handler implementations
 * TODO: Implement these functions to handle each exception type
 */

void handle_supervisor_call_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* For SVC, LR contains PC+4 (return offset is 0), so exception address is lr - 4 */
	print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr - 4);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_undefined_instruction_trampoline(register_context_t* ctx)
{
	/* Print exception information */
	/* For Undefined Instruction, LR contains PC+4 (return offset is 0), so exception address is lr - 4 */
	print_exception_infos(ctx, false, false, "Undefined Instruction", ctx->lr - 4);
	
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
	/* For Data Abort, LR contains PC+8 (return offset is 8), so exception address is lr - 8 (instruction that caused the fault) */
	print_exception_infos(ctx, true, false, "Data Abort", ctx->lr - 8);
	
	/* Halt the system */
	while (true) {
		/* Infinite loop to stop execution */
	}
}

void handle_not_used_trampoline(register_context_t* ctx)
{
	/* TODO: Implement not used handler */
	kprintf("Not Used exception occurred\n");
}

void handle_irq_trampoline(register_context_t* ctx)
{
	/* TODO: Implement IRQ handler */
	kprintf("IRQ occurred\n");
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	/* TODO: Implement FIQ handler */
	kprintf("FIQ occurred\n");
}

