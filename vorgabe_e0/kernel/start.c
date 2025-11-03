#include <config.h>
#include <arch/bsp/uart.h>
#include <lib/kprintf.h>
#include <lib/tests.h>

void start_kernel()
{
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();
	
	// Uncomment the line below to run kprintf tests
	run_kprintf_tests();

	while (true) {
		char c = uart_getc();
		uart_putc(c);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
		kprintf(" Es wurde folgendes Zeichen eingegeben: %c, In Hexadezimal: %x, "
		"In Dezimal: %08i, Als Ptr: %p\n",
		c, (unsigned int)c, (int)c, (void*)c);
#pragma GCC diagnostic pop
	}
}
