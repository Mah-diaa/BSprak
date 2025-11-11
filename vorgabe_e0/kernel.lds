
ENTRY(_start)
SECTIONS
{
	. = 0x00008000;
	.init : { *(.init) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	. = ALIGN(1<<20);
	.data : { *(.data) }
	.bss  : { *(.bss)  }
	
	/* Stack definitions - 1 KiB (0x400) per stack, descending */
	. = 0x100000;
	_supervisor_stack_start = .;
	. = . + 0x400;
	_supervisor_stack_end = .;
	
	_irq_stack_start = .;
	. = . + 0x400;
	_irq_stack_end = .;
	
	_abort_stack_start = .;
	. = . + 0x400;
	_abort_stack_end = .;
	
	_undefined_stack_start = .;
	. = . + 0x400;
	_undefined_stack_end = .;
	
	_system_stack_start = .;
	. = . + 0x400;
	_system_stack_end = .;
}
