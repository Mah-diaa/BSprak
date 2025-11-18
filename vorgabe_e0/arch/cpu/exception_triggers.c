#include <arch/cpu/exception_triggers.h>

void do_supervisor_call(void)
{
	asm volatile("svc #0");
}

void do_undefined_inst(void)
{
	asm volatile(".word 0xE7FFFFFF");
}

void do_data_abort(void)
{
	volatile unsigned int *invalid_addr = (volatile unsigned int *)0xFFFFFFFF;
	volatile unsigned int value = *invalid_addr;
	(void)value; // Suppress unused variable warning
}

void do_prefetch_abort(void)
{
	asm volatile(
		"ldr r0, =0xFFFFFFFF\n\t"
		"mov pc, r0"
	);
}