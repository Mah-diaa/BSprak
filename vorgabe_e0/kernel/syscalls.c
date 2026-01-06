#include <kernel/syscalls.h>
#include <arch/cpu/registers.h>
#include <arch/cpu/scheduler.h>
#include <arch/cpu/exception_print.h>
#include <arch/bsp/uart.h>
#include <kernel/threads.h>
#include <lib/kprintf.h>

//special syscall handler for exit syscall on upercase S
void syscall_handler_exit(register_context_t *ctx)
{
	if (ctx->r0 == 'S') {
		print_exception_infos(ctx, false, false, "Supervisor Call", ctx->lr - 4, 0, 0, 0, 0);
		uart_putc('\4');
		while (true) {
		}
	} else {
		scheduler_end_current_thread(ctx);
	}
}

void syscall_handler_putc(register_context_t *ctx)
{
	char c = (char)ctx->r0;
	uart_putc(c);
}

void syscall_handler_getc(register_context_t *ctx)
{
	char c;
	if (uart_try_getc(&c)) {
		ctx->r0 = (unsigned int)c;
	} else {//if uart is blocked or empty
		struct tcb *current = scheduler_get_current_thread();
		if (current) {
			current->state = WAITING;
		}
		ctx->lr -= 4;
		scheduler_schedule(ctx);
	}
}

void syscall_handler_create_thread(register_context_t *ctx)
{
	//r0 = function pointer, r1 = args pointer, r2 = arg_size
	void (*func)(void *) = (void (*)(void *))ctx->r0;
	void *args = (void *)ctx->r1;
	unsigned int arg_size = ctx->r2;
	scheduler_thread_create(func, args, arg_size);
}

void syscall_handler_sleep(register_context_t *ctx)
{
	unsigned int cycles = ctx->r0;
	if (cycles == 0) {
		scheduler_schedule(ctx);
		return;
	}
	struct tcb *current = scheduler_get_current_thread();
	if (current) {
		current->sleep_ticks = cycles;
		current->state = WAITING;
	}
	scheduler_schedule(ctx);
}
