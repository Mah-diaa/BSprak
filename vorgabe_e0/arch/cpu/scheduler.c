#include <kernel/threads.h>
#include <lib/kprintf.h>
#include <lib/string.h>
#include <arch/bsp/uart.h>
#include <stdbool.h>

bool scheduler_enabled = false;

// Idle thread function (in assembly) - declared as external
extern void idle_thread(void);

// Idle thread TCB - special thread with id = -1
static struct tcb idle_tcb = {
    .thread_id = -1,
    .state = RUNNING,
    .stack_base = NULL,
    .user_sp = 0,
    .user_lr = 0
};

// Pointer to current running thread
static struct tcb *current_thread = &idle_tcb;

// Main scheduling function - called from timer IRQ
void scheduler_schedule(register_context_t *ctx) {
    if (!scheduler_enabled) {
        return;
    }

    // Step 1: Free the current thread if it ended
    if (current_thread->state == ENDED && current_thread != &idle_tcb) {
        current_thread->state = FREE;
    }

    // Step 2: Find next READY or RUNNING thread (round-robin with preemption)
    struct tcb *next_thread = NULL;
    int start_id = (current_thread == &idle_tcb) ? 0 : (current_thread->thread_id + 1) % MAX_THREADS;

    // First pass: look for READY threads
    for (int i = 0; i < MAX_THREADS; i++) {
        int id = (start_id + i) % MAX_THREADS;
        if (tcbs[id].state == READY) {
            next_thread = &tcbs[id];
            break;
        }
    }

    // Step 3: If no READY thread found, check if current can continue
    if (next_thread == NULL) {
        // No other READY threads found
        if (current_thread != &idle_tcb && current_thread->state == RUNNING) {
            // Current thread continues running
            return;
        }
        // Otherwise switch to idle
        next_thread = &idle_tcb;
    }

    // Step 4: If same thread, just continue (no context switch)
    if (next_thread == current_thread) {
        if (current_thread != &idle_tcb) {
            current_thread->state = RUNNING;
        }
        return;
    }

    // Step 5: CONTEXT SWITCH - save current, restore next
    // Only print newline if not switching to/from idle
    if (current_thread != &idle_tcb && next_thread != &idle_tcb) {
        kprintf("\n");
    }

    // Save current thread's context (skip if idle)
    if (current_thread != &idle_tcb) {
        // Save general purpose registers from IRQ stack
        current_thread->context = *ctx;

        // Save user mode banked registers
        asm volatile("mrs %0, sp_usr" : "=r"(current_thread->user_sp));
        asm volatile("mrs %0, lr_usr" : "=r"(current_thread->user_lr));

        // Mark as READY if it was running
        if (current_thread->state == RUNNING) {
            current_thread->state = READY;
        }
    }

    // Restore next thread's context
    if (next_thread == &idle_tcb) {
        // Idle: create fresh context pointing to idle_thread
        memset(ctx, 0, sizeof(register_context_t));
        ctx->spsr = 0x13;  // Supervisor mode, IRQs enabled
        ctx->lr = (unsigned int)idle_thread;
    } else {
        // User thread: restore saved context
        *ctx = next_thread->context;

        // Restore user mode banked registers
        asm volatile("msr sp_usr, %0" :: "r"(next_thread->user_sp));
        asm volatile("msr lr_usr, %0" :: "r"(next_thread->user_lr));

        // Mark as running
        next_thread->state = RUNNING;
    }

    // Update current thread pointer
    current_thread = next_thread;
}

// Called from SVC handler when thread exits
void scheduler_end_current_thread(register_context_t *ctx) {
    if (current_thread != &idle_tcb) {
        current_thread->state = ENDED;
    }
    // Immediately reschedule to switch away from ended thread
    scheduler_schedule(ctx);
}

// Enable the scheduler (called after threads are created)
void scheduler_enable(void) {
    current_thread = &idle_tcb;
    idle_tcb.state = RUNNING;
    scheduler_enabled = true;
}

// Get current running thread (returns NULL if idle is running)
struct tcb* scheduler_get_current_thread(void) {
    if (current_thread == &idle_tcb) {
        return NULL;
    }
    return current_thread;
}
