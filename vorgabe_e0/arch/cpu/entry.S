
.section .init

//macro (Hope this works)
.macro exception_trampoline handler_name, return_offset
\handler_name:
    /* Adjust LR before saving if return_offset is not 0 */
    .if \return_offset != 0
    sub lr, lr, #\return_offset
    .endif
    
    /* Save r0-r12 and lr to stack */
    stmfd sp!, {r0-r12, lr}
    
    /* Save SPSR (saved by hardware automatically when exception occurred) */
    mrs r0, spsr
    push {r0}
    
    /* Pass stack pointer to C handler */
    mov r0, sp
    bl handle_\handler_name
    
    /* Restore SPSR */
    pop {r0}
    msr spsr, r0
    
    /* Restore r0-r12 and lr */
    ldmfd sp!, {r0-r12, lr}
    
    /* Return: movs pc, lr restores PC and CPSR from SPSR */
    movs pc, lr
.endm



.global _ivt
.balign 64
_ivt:
	b _start				/* Reset vector */
	ldr pc, _undefined_instruction		/* Undefined instruction */
	ldr pc, _supervisor_call		/* Supervisor call (SVC) */
	ldr pc, _prefetch_abort		/* Prefetch abort */
	ldr pc, _data_abort			/* Data abort */
	ldr pc, _not_used			/* Reserved/Not used */
	ldr pc, _irq				/* IRQ */
	ldr pc, _fiq				/* FIQ */

_undefined_instruction: .word undefined_instruction_trampoline
_supervisor_call: .word supervisor_call_trampoline
_prefetch_abort: .word prefetch_abort_trampoline
_data_abort: .word data_abort_trampoline
_not_used: .word not_used_trampoline
_irq: .word irq_trampoline
_fiq: .word fiq_trampoline



.global _start
_setup_stacks:
	cpsid if		/* Disable interrupts during setup */

	/* Setup Supervisor mode stack first (so we can use push/pop) */
	cps #0x13		/* Switch to SVC mode */
	ldr sp, =_supervisor_stack_end
	push {lr}		/* Save return address on stack (AAPCS compliant) */

	/* Setup IRQ mode stack */
	ldr r0, =_irq_stack_end
	cps #0x12		/* Switch to IRQ mode */
	mov sp, r0
	mov lr, #0		/* Initialize LR */

	/* Setup Abort mode stack */
	ldr r0, =_abort_stack_end
	cps #0x17		/* Switch to ABT mode */
	mov sp, r0
	mov lr, #0		/* Initialize LR */

	/* Setup Undefined mode stack */
	ldr r0, =_undefined_stack_end
	cps #0x1b		/* Switch to UND mode */
	mov sp, r0
	mov lr, #0		/* Initialize LR */

	/* Setup System mode stack (User and System share the same sp) */
	ldr r0, =_system_stack_end
	cps #0x1f		/* Switch to SYS mode */
	mov sp, r0
	mov lr, #0		/* Initialize LR */

	/* Return to Supervisor mode for kernel execution */
	cps #0x13		/* Switch back to SVC mode */
	cpsie i		/* Re-enable interrupts */
	pop {lr}		/* Restore return address from stack (AAPCS compliant) */
	bx lr		/* Return from function */		

_start:
	/* Erkennen ob Hypervisor Mode aktiv */
	mrs r0, cpsr
	and r0, r0, #0x1F
	mov r1, #0x1A	//Modebits = 0x1A = Hypervisor Mode
	cmp r0, r1		//Vergleichen und Statusbits setzen für bedingte Sprünge

	/* Wenn Hypemode erkannt: abschalten (sprung nach _exitHyper)
	 * wenn nicht erkannt: dann weitermachen (weiterlaufen nach _bsprak)
	 */
	beq _exitHyper

	/* Qemu startet immer alle 4 Kerne
	 * Wir lassen alle anderen Kerne endless loopen
	 */
_checkCores:
	/* Id des Cpu Cores Abfragen */
	mrc p15, 0, r0, c0, c0, 5
	/* Falls nicht core 0 Core disablen */
	tst r0, #3
	bne _parkCore

/* not modeled in qemu 6.0 */
_enableAlignCheck:
	/* SCTLR des cp15 laden */
	mrc p15, 0, r0, c1, c0, 0
	/* A-bit (c1[1]) auf 1 setzen */
	orr r0, r0, #0x2
	/* neues SCTLR speichern */
	mcr p15, 0, r0, c1, c0, 0


_bsprak:
	/* Setup all processor mode stacks */
	bl _setup_stacks
	
	/* Setup Vector Base Address Register (VBAR) */
	ldr r0, =_ivt
	mcr p15, 0, r0, c12, c0, 0	/* Write to VBAR (c12, c0, 0) */
	
	/* Stack is now set, continue to C code */
	bl  start_kernel
.Lend:
	WFI
	b .Lend

_parkCore:
	/* Interrupts für Core 1 bis 3 ausschalten  */
	cpsid if
	/* In Endlosschleife springen */
	b .Lend

_exitHyper:

	/* Rücksprungadresse ins Hypervisor Rücksprungregister schreiben.
	 * Dies ist das Label _bsprak für die normale Programmausfürung.
	 */
	ldr lr, =_checkCores
	msr ELR_hyp, lr

	/* Der Hypervisor mode ist auch "nur" eine etwas kompliziertere Exception.
	 * Also springen wir aus dieser zurück.
	 * (Rücksprunge in Aufgabe 2 sind anders zu lösen!).
	 * Dazu zunächst das CPSR auslesen und die Modebits zum Supervisor ändern.
	 * Danach dies in das SPSR für den Rücksprung schreiben.
	 */
	mrs r0, cpsr
	bic r0, r0, #0x1F
	orr r0, r0, #0x13	//Supervisormode
	msr spsr_hyp, r0
	eret

/*
 * read_sp_from_mode - Read stack pointer from a specific processor mode
 * Input: r0 = mode (enum psr_mode value)
 * Output: r0 = stack pointer value from that mode
 */
.global read_sp_from_mode
read_sp_from_mode:
	/* Save current CPSR and LR to registers (can't use stack across mode switches) */
	mrs r2, cpsr
	mov r3, lr
	
	/* Prepare new CPSR: preserve flags, disable interrupts, change only mode */
	/* r0 contains the mode value (e.g., 0x13 for SVC) */
	/* Clear mode bits and interrupt bits, then set new mode and disable interrupts */
	bic r1, r2, #0x1F
	bic r1, r1, #0xC0	/* Clear I and F bits */
	orr r1, r1, r0		/* Set new mode */
	orr r1, r1, #0xC0	/* Set I and F bits to disable interrupts */
	
	/* Switch to requested mode with interrupts disabled */
	msr cpsr, r1
	
	/* Read SP from the requested mode */
	mov r1, sp
	
	/* Restore original CPSR (this restores mode, flags, and interrupt state) */
	msr cpsr, r2
	
	/* Move SP value to return register */
	mov r0, r1
	
	/* Restore LR and return */
	mov lr, r3
	bx lr

exception_trampoline supervisor_call_trampoline, 4
exception_trampoline undefined_instruction_trampoline, 4
exception_trampoline prefetch_abort_trampoline, 4
exception_trampoline data_abort_trampoline, 8
exception_trampoline not_used_trampoline, 0
exception_trampoline irq_trampoline, 4
exception_trampoline fiq_trampoline, 4
