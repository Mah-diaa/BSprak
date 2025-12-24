#pragma once

/**
 * \file syscalls.h (user-space)
 *
 * User-space system call API
 *
 * These functions can be called from user-space threads to interact
 * with the kernel via the SVC (Supervisor Call) mechanism.
 */

/* Syscall numbers (must match kernel definition) */
#define SYSCALL_EXIT            0
#define SYSCALL_PUTC            1
#define SYSCALL_GETC            2
#define SYSCALL_CREATE_THREAD   3
#define SYSCALL_SLEEP           4

/**
 * Terminate the calling thread
 * Does not return.
 */
[[noreturn]] void syscall_exit(void);

/**
 * Output a single character
 * @param c Character to output
 */
void syscall_putc(char c);

/**
 * Read a single character (blocking)
 * Blocks until a character is available.
 * @return The character read
 */
char syscall_getc(void);

/**
 * Create a new thread
 * @param f Thread entry point function
 * @param args Pointer to argument data
 * @param arg_size Size of argument data in bytes
 */
void syscall_create_thread(void (*f)(void *), void *args, unsigned int arg_size);

/**
 * Sleep for a specified number of timer ticks (blocking)
 * The thread will not be scheduled for at least 'cycles' timer interrupts.
 * @param cycles Number of timer ticks to sleep
 */
void syscall_sleep(unsigned int cycles);

/**
 * Trigger an undefined syscall (for testing)
 * This will terminate the thread and print a register dump.
 */
void syscall_undefined(void);
