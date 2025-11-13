
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
	
	/* Stack definitions - 2 KiB (0x800) per stack, descending order */
	/* SVC stack: 0x100000 - 0xFF800 */
	. = 0x100000;
	_supervisor_stack_end = .;
	_supervisor_stack_start = . - 0x800;
	
	/* UND stack: 0xFF800 - 0xFF000 */
	. = 0xFF800;
	_undefined_stack_end = .;
	_undefined_stack_start = . - 0x800;
	
	/* ABT stack: 0xFF000 - 0xFE800 */
	. = 0xFF000;
	_abort_stack_end = .;
	_abort_stack_start = . - 0x800;
	
	/* IRQ stack: 0xFE800 - 0xFE000 */
	. = 0xFE800;
	_irq_stack_end = .;
	_irq_stack_start = . - 0x800;
	
	/* SYS stack: 0xFE000 - 0xFD800 */
	. = 0xFE000;
	_system_stack_end = .;
	_system_stack_start = . - 0x800;
}
