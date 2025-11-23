#include "arch/bsp/yellow_led.h"
#include "arch/cpu/registers.h"
#include <arch/cpu/exception_print.h>
#include <arch/cpu/psr.h>
#include <lib/kprintf.h>
const char* get_fsr_description(unsigned int fsr){
    static const char *fsr_sources[] = {
        [0b00000] =  "No function, reset value",
        [0b00001] =  "Alignment fault",
        [0b00010] =  "Debug event fault",
        [0b00011] =  "Access Flag fault on Section",
        [0b00100] =  "Cache maintenance operation fault",
        [0b00101] =  "Translation fault on Section",
        [0b00110] =  "Access Flag fault on Page",
        [0b00111] =  "Translation fault on Page",
        [0b01000] =  "Precise External Abort",
        [0b01001] =  "Domain fault on Section",
        [0b01011] =  "Domain fault on Page",
        [0b01100] =  "External abort on Section",
        [0b01101] =  "Permission fault on Section",
        [0b01110] =  "External abort on Page",
        [0b01111] =  "Permission fault on Page",
        [0b10000] =  "TLB conflict abort",
        [0b10100] =  "Implementation definedf fault",
        [0b10110] =  "External Abort",
        [0b11000] =  "Asynchronous parity error on memory access",
        [0b11001] =  "Synchronous parity error on memory access",
        [0b11010] =  "Implementation defined fault",
        [0b11100] =  "Synchronous parity error on translation table walk on section",
        [0b11110] =  "Synchronous parity error on translation table walk on page",
    };

    constexpr int fsr_status_4_index = 10;
    unsigned int fsr_status = (fsr & 0b1111) |
     ((fsr & (1 << (fsr_status_4_index - 1))) >> (fsr_status_4_index - 4 - 1));

    if(fsr_status > sizeof(fsr_sources) / sizeof(const char*) ||
       fsr_sources[fsr_status] == NULL){
        return "Invalid fault status register value";
    }

    return fsr_sources[fsr_status];
}
char *get_mode_name(unsigned int mode) {
	switch (mode) {
		case PSR_USR: return "User";
		case PSR_FIQ: return "FIQ";
		case PSR_IRQ: return "IRQ";
		case PSR_SVC: return "Supervisor";
		case PSR_ABT: return "Abort";
		case PSR_UND: return "Undefined";
		case PSR_SYS: return "System";
		default: return "Invalid";
	}
}
void print_psr(unsigned int psr) {
	int psr_n = (psr & PSR_N) != 0;
	int psr_z = (psr & PSR_Z) != 0;
	int psr_c = (psr & PSR_C) != 0;
	int psr_v = (psr & PSR_V) != 0;
	int psr_e = (psr & PSR_E) != 0;
	int psr_i = (psr & PSR_I) != 0;
	int psr_f = (psr & PSR_F) != 0;
	int psr_t = (psr & PSR_T) != 0;

	unsigned int mode = psr & PSR_MODE_MASK;

	kprintf("%c%c%c%c %c %c%c%c",
		psr_n ? 'N' : '_',
		psr_z ? 'Z' : '_',
		psr_c ? 'C' : '_',
		psr_v ? 'V' : '_',
		psr_e ? 'E' : '_',
		psr_i ? 'I' : '_',
		psr_f ? 'F' : '_',
		psr_t ? 'T' : '_'
	);

	char *right_aligned_mode_name = get_mode_name(mode);
	kprintf(" %s", right_aligned_mode_name);
	kprintf(" 0x%08x", psr);
}
mode_regs_t read_mode_specific_registers(register_context_t* ctx)
{
	mode_regs_t regs;
	unsigned int original_cpsr;
	unsigned int current_cpsr;
	unsigned int current_mode;

	asm volatile("mrs %0, cpsr" : "=r" (original_cpsr));
	asm volatile("mrs %0, cpsr" : "=r" (current_cpsr));
	current_mode = current_cpsr & PSR_MODE_MASK;

	asm volatile("cpsid if"); //disable interrupts

	// "i" tells gcc to use a compile time constant, pretty neat
	asm volatile("cps %0" : : "i" (PSR_SYS));
	asm volatile("mov %0, sp" : "=r" (regs.user_sp));
	asm volatile("mov %0, lr" : "=r" (regs.user_lr));
	asm volatile("mrs %0, cpsr" : "=r" (regs.user_cpsr));

	//Read IRQ mode
	asm volatile("cps %0" : : "i" (PSR_IRQ));
	asm volatile("mov %0, sp" : "=r" (regs.irq_sp));
	asm volatile("mov %0, lr" : "=r" (regs.irq_lr));
	asm volatile("mrs %0, spsr" : "=r" (regs.irq_spsr));

	//Read Abort mode
	asm volatile("cps %0" : : "i" (PSR_ABT));
	asm volatile("mov %0, sp" : "=r" (regs.abort_sp));
	asm volatile("mov %0, lr" : "=r" (regs.abort_lr));
	asm volatile("mrs %0, spsr" : "=r" (regs.abort_spsr));

	//Read Undefined mode
	asm volatile("cps %0" : : "i" (PSR_UND));
	asm volatile("mov %0, sp" : "=r" (regs.undefined_sp));
	asm volatile("mov %0, lr" : "=r" (regs.undefined_lr));
	asm volatile("mrs %0, spsr" : "=r" (regs.undefined_spsr));

	//Read Supervisor mode
	asm volatile("cps %0" : : "i" (PSR_SVC));
	asm volatile("mov %0, sp" : "=r" (regs.supervisor_sp));
	asm volatile("mov %0, lr" : "=r" (regs.supervisor_lr));
	asm volatile("mrs %0, spsr" : "=r" (regs.supervisor_spsr));

	//go back
	asm volatile("msr cpsr_cxsf, %0" : : "r" (original_cpsr));

	if (ctx) {
		if (current_mode == PSR_SVC) {
			regs.supervisor_lr = ctx->lr;
			regs.supervisor_spsr = ctx->spsr;
			regs.user_cpsr = current_cpsr;
		} else if (current_mode == PSR_IRQ) {
			regs.irq_lr = ctx->lr;
			regs.irq_spsr = ctx->spsr;
			regs.user_cpsr = current_cpsr;
		} else if (current_mode == PSR_ABT) {
			regs.abort_lr = ctx->lr;
			regs.abort_spsr = ctx->spsr;
			regs.user_cpsr = current_cpsr;
		} else if (current_mode == PSR_UND) {
			regs.undefined_lr = ctx->lr;
			regs.undefined_spsr = ctx->spsr;
			regs.user_cpsr = current_cpsr;
		}
	}

	return regs;
}

void print_exception_infos(register_context_t* ctx, bool is_data_abort, bool is_prefetch_abort, char* exception_name, unsigned int exception_source_addr) {
	kprintf("############ EXCEPTION ############\n");
	kprintf("%s an Adresse: 0x%08x\n", exception_name, exception_source_addr);
	if(is_data_abort) {
		const char* dfsr_description = get_fsr_description(ctx->dfsr);
		kprintf("Data Fault Status Register: 0x%08x -> %s\n", ctx->dfsr, dfsr_description);
		kprintf("Data Fault Adress Register: 0x%08x\n", ctx->dfar);
	}
		
	if(is_prefetch_abort) {
		const char* ifsr_description = get_fsr_description(ctx->ifsr);
		kprintf("Instruction Fault Status Register: 0x%08x -> %s\n", ctx->ifsr, ifsr_description);
		kprintf("Instruction Fault Adress Register: 0x%08x\n", ctx->ifar);
	}
			
	kprintf("\n>> Registerschnappschuss <<\n");
    kprintf("R0: 0x%08x  R5: 0x%08x  R10: 0x%08x\n", ctx->r0, ctx->r5, ctx->r10);
	kprintf("R1: 0x%08x  R6: 0x%08x  R11: 0x%08x\n", ctx->r1, ctx->r6, ctx->r11);
	kprintf("R2: 0x%08x  R7: 0x%08x  R12: 0x%08x\n", ctx->r2, ctx->r7, ctx->r12);
	kprintf("R3: 0x%08x  R8: 0x%08x\n", ctx->r3, ctx->r8);
	kprintf("R4: 0x%08x  R9: 0x%08x\n", ctx->r4, ctx->r9);

	struct mode_regs mode_regs = read_mode_specific_registers(ctx);

	kprintf("\n>> Modusspezifische Register <<\n");
	kprintf("User/System | LR: 0x%08x | SP: 0x%08x | CPSR: ", mode_regs.user_lr, mode_regs.user_sp);
	print_psr(mode_regs.user_cpsr);
	kprintf("\nIRQ         | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.irq_lr, mode_regs.irq_sp);
	print_psr(mode_regs.irq_spsr);
	kprintf("\nAbort       | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.abort_lr, mode_regs.abort_sp);
	print_psr(mode_regs.abort_spsr);
	kprintf("\nUndefined   | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.undefined_lr, mode_regs.undefined_sp);
	print_psr(mode_regs.undefined_spsr);
	kprintf("\nSupervisor  | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.supervisor_lr, mode_regs.supervisor_sp);
	print_psr(mode_regs.supervisor_spsr);
	kprintf("\n");}
