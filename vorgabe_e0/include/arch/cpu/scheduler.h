#pragma once

#include <arch/cpu/registers.h>
#include <kernel/threads.h>

void scheduler_schedule(register_context_t *ctx);
void scheduler_end_current_thread(register_context_t *ctx);
void scheduler_enable(void);
struct tcb* scheduler_get_current_thread(void);
void scheduler_wake_waiting_threads(void);  /* Wake threads in WAITING state */
void scheduler_tick_sleep(void);  /* Decrement sleep counters on timer interrupt */
