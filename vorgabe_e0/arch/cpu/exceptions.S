.section .init

.macro exception_trampoline handler_name, return_offset
\handler_name:
	.if \return_offset != 0
	sub lr, lr, #\return_offset @ For IRQ/FIQ: hardware sets LR = last completed + 8, but we need interrupted + 4
	.endif
	stmfd sp!, {r0-r12, lr} @ Save LR pointing to continuation address
	mrs r0, spsr
	stmfd sp!, {r0}
	mov r0, sp
	bl handle_\handler_name
	ldmfd sp!, {r0}
	msr spsr_cxsf, r0
	ldmfd sp!, {r0-r12, lr}
	movs pc, lr
.endm

.global _ivt
.balign 64
_ivt:
	b _start
	ldr pc, _undefined_instruction
	ldr pc, _supervisor_call
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undefined_instruction: .word undefined_instruction_trampoline
_supervisor_call: .word supervisor_call_trampoline
_prefetch_abort: .word prefetch_abort_trampoline
_data_abort: .word data_abort_trampoline
_not_used: .word not_used_trampoline
_irq: .word irq_trampoline
_fiq: .word fiq_trampoline

exception_trampoline supervisor_call_trampoline, 0 @ Hardware sets LR = triggering + 4
exception_trampoline undefined_instruction_trampoline, 0
exception_trampoline prefetch_abort_trampoline, 0
exception_trampoline data_abort_trampoline, 0 @ Hardware sets LR = triggering + 8
exception_trampoline not_used_trampoline, 0
exception_trampoline irq_trampoline, 4 @Hardware sets LR = last completed + 8, subtract 4 to get interrupted + 4
exception_trampoline fiq_trampoline, 4

// Idle thread - runs when no other threads are ready
.global idle_thread
idle_thread:
	nop             @ Do nothing
	b idle_thread   @ Loop forever 
