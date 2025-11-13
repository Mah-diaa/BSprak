#include <arch/cpu/stack.h>
#include <arch/cpu/psr.h>
#include <lib/kprintf.h>

/* Assembly function to read SP from a mode */
extern unsigned int read_sp_from_mode(enum psr_mode mode);

void verify_stacks(void)
{
	unsigned int sp_svc, sp_und, sp_abt, sp_irq, sp_sys;
	unsigned int expected_svc = 0x100000;
	unsigned int expected_und = 0xFFC00;
	unsigned int expected_abt = 0xFF800;
	unsigned int expected_irq = 0xFF400;
	unsigned int expected_sys = 0xFF000;

	kprintf("\n=== Verifying Stack Pointers ===\n");

	/* Read stack pointers from each mode */
	sp_svc = read_sp_from_mode(PSR_SVC);
	sp_und = read_sp_from_mode(PSR_UND);
	sp_abt = read_sp_from_mode(PSR_ABT);
	sp_irq = read_sp_from_mode(PSR_IRQ);
	sp_sys = read_sp_from_mode(PSR_SYS);

	/* Print results */
	kprintf("SVC stack: 0x%08x (expected: 0x%08x) %s\n",
		sp_svc, expected_svc, (sp_svc == expected_svc) ? "✓" : "✗");
	kprintf("UND stack: 0x%08x (expected: 0x%08x) %s\n",
		sp_und, expected_und, (sp_und == expected_und) ? "✓" : "✗");
	kprintf("ABT stack: 0x%08x (expected: 0x%08x) %s\n",
		sp_abt, expected_abt, (sp_abt == expected_abt) ? "✓" : "✗");
	kprintf("IRQ stack: 0x%08x (expected: 0x%08x) %s\n",
		sp_irq, expected_irq, (sp_irq == expected_irq) ? "✓" : "✗");
	kprintf("SYS stack: 0x%08x (expected: 0x%08x) %s\n",
		sp_sys, expected_sys, (sp_sys == expected_sys) ? "✓" : "✗");

	/* Check spacing (each should be 0x400 apart) */
	kprintf("\n=== Checking Stack Spacing (1 KiB = 0x400) ===\n");
	kprintf("SVC -> UND: 0x%x (expected: 0x400) %s\n",
		sp_svc - sp_und, (sp_svc - sp_und == 0x400) ? "✓" : "✗");
	kprintf("UND -> ABT: 0x%x (expected: 0x400) %s\n",
		sp_und - sp_abt, (sp_und - sp_abt == 0x400) ? "✓" : "✗");
	kprintf("ABT -> IRQ: 0x%x (expected: 0x400) %s\n",
		sp_abt - sp_irq, (sp_abt - sp_irq == 0x400) ? "✓" : "✗");
	kprintf("IRQ -> SYS: 0x%x (expected: 0x400) %s\n",
		sp_irq - sp_sys, (sp_irq - sp_sys == 0x400) ? "✓" : "✗");

	kprintf("\n");
}

