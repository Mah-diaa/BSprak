#include <user/syscalls.h>

void syscall_exit(void)
{
	asm volatile(
		"mov r7, %0\n"
		"mov r0, #0\n"
		"svc #0\n"
		:
		: "i"(SYSCALL_EXIT)
		: "r0", "r7"
	);
	__builtin_unreachable();
	//this is a compiler builtin, basically acts as [[noreturn]] but inline ~M
	//https://stackoverflow.com/questions/54764535/what-optimizations-does-builtin-unreachable-facilitate
}

//special wrapper for when S is entered
void syscall_exit_shutdown(char c)
{
	asm volatile(
		"mov r7, %1\n"
		"mov r0, %0\n"  //
		"svc #0\n"
		:
		: "r"(c), "i"(SYSCALL_EXIT)
		: "r0", "r7"
	);
	__builtin_unreachable();
}

void syscall_putc(char c)
{
	asm volatile(
		"mov r7, %1\n"
		"mov r0, %0\n"
		"svc #0\n"
		:
		: "r"(c), "i"(SYSCALL_PUTC)
		: "r0", "r7"
	);
}

char syscall_getc(void)
{
	char result;
	asm volatile(
		"mov r7, %1\n"
		"svc #0\n"
		"mov %0, r0\n"
		: "=r"(result)
		: "i"(SYSCALL_GETC)
		: "r0", "r7"
	);
	return result;
}

void syscall_create_thread(void (*f)(void *), void *args, unsigned int arg_size)
{
	asm volatile(
		"mov r7, %3\n"
		"mov r0, %0\n"
		"mov r1, %1\n"
		"mov r2, %2\n"
		"svc #0\n"
		:
		: "r"(f), "r"(args), "r"(arg_size), "i"(SYSCALL_CREATE_THREAD)
		: "r0", "r1", "r2", "r7"
	);
}

void syscall_sleep(unsigned int cycles)
{
	asm volatile(
		"mov r7, %1\n"
		"mov r0, %0\n"
		"svc #0\n"
		:
		: "r"(cycles), "i"(SYSCALL_SLEEP)
		: "r0", "r7"
	);
}

void syscall_undefined(void)
{
	asm volatile(
		"mov r7, #255\n"
		"svc #0\n"
		:
		:
		: "r7"
	);
}
