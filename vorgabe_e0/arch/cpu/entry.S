.section .init

.global _start
_setup_stacks:
	cpsid if
	cps #0x13
	ldr sp, =_supervisor_stack_end
	push {lr}
	ldr r0, =_irq_stack_end
	cps #0x12
	mov sp, r0
	mov lr, #0
	ldr r0, =_abort_stack_end
	cps #0x17
	mov sp, r0
	mov lr, #0
	ldr r0, =_undefined_stack_end
	cps #0x1b
	mov sp, r0
	mov lr, #0
	ldr r0, =_system_stack_end
	cps #0x1f
	mov sp, r0
	mov lr, #0
	cps #0x13
	cpsie i
	pop {lr}
	bx lr		

_start:
	mrs r0, cpsr
	and r0, r0, #0x1F
	mov r1, #0x1A
	cmp r0, r1
	beq _exitHyper

_checkCores:
	mrc p15, 0, r0, c0, c0, 5
	tst r0, #3
	bne _parkCore

_enableAlignCheck:
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #0x2
	mcr p15, 0, r0, c1, c0, 0

_bsprak:
	bl _setup_stacks
	ldr r0, =_ivt
	mcr p15, 0, r0, c12, c0, 0
	bl start_kernel
.Lend:
	WFI
	b .Lend

_parkCore:
	cpsid if
	b .Lend

_exitHyper:
	ldr lr, =_checkCores
	msr ELR_hyp, lr
	mrs r0, cpsr
	bic r0, r0, #0x1F
	orr r0, r0, #0x13
	msr spsr_hyp, r0
	eret
