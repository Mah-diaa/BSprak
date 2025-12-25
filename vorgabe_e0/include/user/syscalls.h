#pragma once
#define SYSCALL_EXIT            0
#define SYSCALL_PUTC            1
#define SYSCALL_GETC            2
#define SYSCALL_CREATE_THREAD   3
#define SYSCALL_SLEEP           4

[[noreturn]] void syscall_exit(void);
[[noreturn]] void syscall_exit_shutdown(char c);
void syscall_putc(char c);
char syscall_getc(void);
void syscall_create_thread(void (*f)(void *), void *args, unsigned int arg_size);
void syscall_sleep(unsigned int cycles);
void syscall_undefined(void);
