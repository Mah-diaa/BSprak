#pragma once

#include <arch/cpu/registers.h>
#include <kernel/threads.h>

// Main scheduler function - called from IRQ handler to perform context switch
void scheduler_schedule(register_context_t *ctx);

// End current thread and reschedule - called from SVC handler
void scheduler_end_current_thread(register_context_t *ctx);

// Enable scheduler after threads are created
void scheduler_enable(void);

// Get currently running thread (returns NULL if no thread running)
struct tcb* scheduler_get_current_thread(void);
