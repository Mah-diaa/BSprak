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
	/* Print exception information if debug is enabled */
	/* For IRQ, LR contains PC+IRQ_RETURN_OFFSET (return offset is IRQ_RETURN_OFFSET), 
	 * so exception address is lr - IRQ_RETURN_OFFSET */
	if (irq_debug) {
		print_exception_infos(ctx, false, false, "IRQ", ctx->lr - IRQ_RETURN_OFFSET);
	}
	
	/* Handle interrupt sources - check IRQ controller to see which interrupt fired */
	irq_controller_handler();
	
	/* Adjust LR in the context for proper return
	 * The trampoline will restore registers and return, but it uses 'subs pc, lr, #offset'
	 * which doesn't restore CPSR from SPSR. So we need to do our own return.
	 * 
	 * For IRQ, LR contains PC+IRQ_RETURN_OFFSET, so we subtract IRQ_RETURN_OFFSET
	 * to get the correct return address.
	 */
	ctx->lr -= IRQ_RETURN_OFFSET;
	
	/* Now restore registers and return, restoring CPSR from SPSR
	 * We must be in IRQ mode to access SPSR_irq
	 * ctx points to the stack frame containing saved r0-r12, lr
	 */
	unsigned int lr_offset = LR_STACK_OFFSET;
	
	/* Ensure we're in IRQ mode (0x12) for SPSR_irq access */
	asm volatile("cps #0x12" ::: "memory");
	
	/* Restore all registers and return, restoring CPSR from SPSR
	 * Use r1 as temp base since r0 is in the restore list
	 * The '^' suffix with PC restores CPSR from SPSR
	 */
	asm volatile(
		"mov r1, %0\n\t"               /* Use r1 as temp base (r0 will be restored) */
		"ldmfd r1!, {r0-r12, lr}\n\t"  /* Restore r0-r12 and lr from stack */
		"movs pc, lr\n\t"              /* Return and restore CPSR from SPSR */
		:
		: "r" (ctx), "r" (lr_offset)
		: "r1", "memory"
	);
	
	/* This should never be reached */
}

void handle_fiq_trampoline(register_context_t* ctx)
{
	/* TODO: Implement FIQ handler */
	kprintf("FIQ occurred\n");
}

