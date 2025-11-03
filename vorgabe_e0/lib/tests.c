#include <config.h>
#include <lib/kprintf.h>
#include <limits.h>

void run_kprintf_tests(void)
{
	kprintf("A:%8i:B\n", 0);
	kprintf("A:%8i:B\n", 467294);
	kprintf("A:%08i:B\n", 467294);
	kprintf("A:%8i:B\n", -12345);
	kprintf("A:%08i:B\n", -12345);
	kprintf("A:%8i:B\n", 123456789);

	kprintf("A:%8u:B\n", 0u);
	kprintf("A:%8u:B\n", 123u);
	kprintf("A:%08u:B\n", 123u);
	kprintf("A:%8u:B\n", 3000000000u);

	kprintf("A:%8x:B\n", 0x0u);
	kprintf("A:%8x:B\n", 0x7au);
	kprintf("A:%08x:B\n", 0x7au);
	kprintf("A:%8x:B\n", 0xffffffffu);
	kprintf("A:%08x:B\n", 0xffffffffu);

	kprintf(":%8i\nu:%u\nx:%08x\n", 123, 123u, 0x7au);

	kprintf("neg:%08i\n", -1);
	kprintf("neg:%08i\n", INT_MIN);

	kprintf("%08i %i\n", 42, 42);
	kprintf("%8i %08i\n", 7, 7);

	kprintf("mix:%8i|%08u|%8x|\n", -123, 456u, 0xABCu);

	kprintf("s:%8s|\n", "hi");
	kprintf("s:%08s|\n", "hi");
	kprintf("c:%c|\n", 'A');
	kprintf("c:%c|\n", 65);

	kprintf("pct:%% done\n");

	kprintf("%8i\n", 2147483647);
	kprintf("%8i\n", -2147483648);
	kprintf("%08u\n", 0u);
	kprintf("%8x\n", 0u);

	kprintf("a:%8i b:%08u c:%8x d:%8i\n", -12, 34u, 0x5au, 789);
	kprintf("Testing i: %i \n", 467294);
	kprintf("Testing 8i: %8i \n", 467294);
	kprintf("Testing 08i: %08i \n", 467294);
	kprintf("Testing i1: %i  i2: %i \n", 1, 2);
	kprintf("Testing negative i: %i \n", -123124);
	kprintf("Testing u: %u \n", 4294967294U);
	kprintf("Testing x: %x\n", 112334);
	kprintf("Testing % : %%\n");

	kprintf("Testing string: %s \n", "I AM A STRING I AM STRINGY");
	kprintf("Testing char: %c \n", 49);

	kprintf("NULL pointer: %p\n", (void*)0);
	kprintf("Small pointer: %p\n", (void*)0x7A);
	kprintf("Large pointer: %p\n", (void*)0xFFFFFFFF);
	kprintf("Current function: %p\n", (void*)run_kprintf_tests);

	// Test % edge cases cus im done 
	 kprintf("\n=== Testing %% (literal percent) ===\n");
	kprintf("Simple %%: %%\n");
	kprintf("Start with %%: %%test\n");
	kprintf("End with %%: test%%\n");
	kprintf("Multiple %%: %% %% %%\n");
	kprintf("%% with number: 50%%\n");
	kprintf("Mixed: %i%% complete\n", 75);
	kprintf("Mixed: %%i prints: %i\n", 42);
	kprintf("Mixed: %s is 100%% correct\n", "This");
	kprintf("%% with field width: %%%8i%%\n", 123);
	kprintf("%% with zero pad: %%%08i%%\n", 456);
	kprintf("Complex: Value %i%% of %u = %x%%\n", 50, 200u, 0x64u);
	kprintf("Edge: %%%%\n");  
	kprintf("Edge: %%%%test\n");
	kprintf("Edge: test%%%%\n");
	kprintf("Done with %% tests\n\n");
}

