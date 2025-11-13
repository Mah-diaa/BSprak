#include <arch/cpu/exception_triggers.h>

/* Trigger a Supervisor Call (SVC) exception */
void do_supervisor_call(void)
{
	/* SVC instruction with immediate value 0 */
	asm volatile("svc #0");
}

/* Trigger an Undefined Instruction exception */
void do_undefined_inst(void)
{
	/* Invalid instruction that will cause undefined instruction exception */
	asm volatile(".word 0xE7FFFFFF");
}

/* Trigger a Data Abort exception */
void do_data_abort(void)
{
	/* Try to read from an invalid memory address */
	volatile unsigned int *invalid_addr = (volatile unsigned int *)0xFFFFFFFF;
	volatile unsigned int value = *invalid_addr;
	(void)value; /* Suppress unused variable warning */
}

/* Trigger a Prefetch Abort exception */
void do_prefetch_abort(void)
{
	/* Load invalid address and branch to it to cause prefetch abort */
	asm volatile(
		"ldr r0, =0xFFFFFFFF\n\t"
		"mov pc, r0"
	);
}

