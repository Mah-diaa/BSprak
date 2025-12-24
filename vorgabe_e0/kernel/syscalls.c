#include <kernel/syscalls.h>
#include <arch/cpu/registers.h>
#include <arch/cpu/scheduler.h>
#include <arch/bsp/uart.h>
#include <kernel/threads.h>
#include <lib/kprintf.h>

/**
 * Syscall handler implementations
 *
 * These functions are called from the SVC exception handler.
 * Arguments are passed via registers r0-r3 in the context.
 * Return values (if any) should be placed in ctx->r0.
 */

void syscall_handler_exit(register_context_t *ctx)
{
	// Terminate the calling thread
	scheduler_end_current_thread(ctx);
}

void syscall_handler_putc(register_context_t *ctx)
{
	// char c is in r0
	char c = (char)ctx->r0;
	uart_putc(c);
}

void syscall_handler_getc(register_context_t *ctx)
{
	char c;
	// Try to get a character
	if (uart_try_getc(&c)) {
		// Character available - return it in r0
		ctx->r0 = (unsigned int)c;
	} else {
		// No character available - block this thread
		struct tcb *current = scheduler_get_current_thread();
		if (current) {
			current->state = WAITING;
		}

		// Adjust return address to re-execute the SVC instruction
		// When thread wakes up, it will retry syscall_getc
		ctx->lr -= 4;  // Move back to the SVC instruction

		// Scheduler will switch to another thread
		// When this thread is woken and scheduled again, it will retry the syscall
		scheduler_schedule(ctx);
	}
}

void syscall_handler_create_thread(register_context_t *ctx)
{
	// Arguments: r0 = function pointer, r1 = args pointer, r2 = arg_size
	void (*func)(void *) = (void (*)(void *))ctx->r0;
	void *args = (void *)ctx->r1;
	unsigned int arg_size = ctx->r2;

	// Call existing thread creation function
	scheduler_thread_create(func, args, arg_size);
}

void syscall_handler_sleep(register_context_t *ctx)
{
	unsigned int cycles = ctx->r0;

	// Handle sleep(0) - just yield to scheduler without blocking
	if (cycles == 0) {
		scheduler_schedule(ctx);
		return;
	}

	// Block thread for specified number of timer ticks
	struct tcb *current = scheduler_get_current_thread();
	if (current) {
		current->sleep_ticks = cycles;
		current->state = WAITING;
	}

	// Yield to scheduler
	scheduler_schedule(ctx);
}
