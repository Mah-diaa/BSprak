#pragma once

#include <arch/cpu/registers.h>

/**
 * \file syscalls.h
 *
 * System Call Interface Definition
 *
 * Calling Convention for System Calls via SVC:
 * ============================================
 *
 * System calls are invoked using the SVC (Supervisor Call) instruction.
 *
 * Register Usage:
 * ---------------
 * - r7: Syscall number (identifies which syscall to execute)
 * - r0-r3: Arguments (up to 4 arguments, additional args use stack)
 * - r0: Return value (if applicable)
 *
 * Example:
 *   To call syscall_putc('A'):
 *     mov r7, #SYSCALL_PUTC
 *     mov r0, #'A'
 *     svc #0
 *
 * The SVC handler will:
 * 1. Check that the call came from user mode (not kernel mode)
 * 2. Dispatch to the appropriate syscall handler based on r7
 * 3. Pass arguments from r0-r3 to the handler
 * 4. Return value in r0 (if applicable)
 * 5. Return to user mode
 *
 * Blocking Syscalls:
 * ------------------
 * Some syscalls (getc, sleep) may block. When blocked:
 * - Thread state changes to WAITING
 * - Scheduler will not run this thread until unblocked
 * - For getc: unblock when character available in ringbuffer
 * - For sleep: unblock after specified number of timer ticks
 */

/* Syscall numbers */
#define SYSCALL_EXIT            0
#define SYSCALL_PUTC            1
#define SYSCALL_GETC            2
#define SYSCALL_CREATE_THREAD   3
#define SYSCALL_SLEEP           4

/* Maximum valid syscall number */
#define SYSCALL_MAX             4

/* Syscall handler function prototypes (kernel-side implementations) */
void syscall_handler_exit(register_context_t *ctx);
void syscall_handler_putc(register_context_t *ctx);
void syscall_handler_getc(register_context_t *ctx);
void syscall_handler_create_thread(register_context_t *ctx);
void syscall_handler_sleep(register_context_t *ctx);
