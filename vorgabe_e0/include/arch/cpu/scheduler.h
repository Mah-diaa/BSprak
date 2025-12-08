#pragma once

#include <arch/cpu/registers.h>
#include <kernel/threads.h>

void scheduler_schedule(register_context_t *ctx);
void scheduler_end_current_thread(register_context_t *ctx);
void scheduler_enable(void);
struct tcb* scheduler_get_current_thread(void);
