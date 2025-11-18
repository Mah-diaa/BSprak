#ifndef REGISTERS_H_
#define REGISTERS_H_

typedef struct register_context{
	unsigned int spsr;
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int lr;

	unsigned int pc;

	unsigned int sp;

	unsigned int cpsr;

	unsigned int dfsr;
	unsigned int dfar;
	unsigned int ifsr;
	unsigned int ifar;
}register_context_t;

typedef struct mode_regs {
	unsigned int user_lr;
	unsigned int user_sp;
	unsigned int user_cpsr;
	unsigned int irq_lr;
	unsigned int irq_sp;
	unsigned int irq_spsr;
	unsigned int abort_lr;
	unsigned int abort_sp;
	unsigned int abort_spsr;
	unsigned int undefined_lr;
	unsigned int undefined_sp;
	unsigned int undefined_spsr;
	unsigned int supervisor_lr;
	unsigned int supervisor_sp;
	unsigned int supervisor_spsr;
}mode_regs_t;

#endif // REGISTERS_H_
