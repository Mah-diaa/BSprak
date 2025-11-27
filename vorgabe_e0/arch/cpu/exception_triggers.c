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
	volatile unsigned int *unaligned_addr = (volatile unsigned int *)0x00000001;
	volatile unsigned int value = *unaligned_addr;
	(void)value; // suppress unused variable warning
}

void do_prefetch_abort(void)
{
	asm volatile("bkpt #0");
}