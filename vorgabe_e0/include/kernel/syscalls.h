#pragma once

#include <arch/cpu/registers.h>

// Syscall convention:
// * - r7: Syscall number
// * - r0-r3: Arguments
// * - r0: Return value if needed


#define SYSCALL_EXIT            0
#define SYSCALL_PUTC            1
#define SYSCALL_GETC            2
#define SYSCALL_CREATE_THREAD   3
#define SYSCALL_SLEEP           4

#define SYSCALL_MAX             4

void syscall_handler_exit(register_context_t *ctx);
void syscall_handler_putc(register_context_t *ctx);
void syscall_handler_getc(register_context_t *ctx);
void syscall_handler_create_thread(register_context_t *ctx);
void syscall_handler_sleep(register_context_t *ctx);
