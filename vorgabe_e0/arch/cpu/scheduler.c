#include <kernel/threads.h>
#include <lib/kprintf.h>
#include <lib/string.h>
#include <arch/bsp/uart.h>
#include <stdbool.h>

bool scheduler_enabled = false;

extern void idle_thread(void);

static struct tcb idle_tcb = {
    .thread_id = -1,
    .state = RUNNING,
    .stack_base = NULL,
    .user_sp = 0,
    .user_lr = 0
};

static struct tcb *current_thread = &idle_tcb;

void scheduler_schedule(register_context_t *ctx) {
    if (!scheduler_enabled) {
        return;
    }
    if (current_thread->state == ENDED && current_thread != &idle_tcb) {
        current_thread->state = FREE;
    }
    struct tcb *next_thread = NULL;
    int start_id = (current_thread == &idle_tcb) ? 0 : (current_thread->thread_id + 1) % MAX_THREADS;//loop aroound if more than 32 threads

    for (int i = 0; i < MAX_THREADS; i++) {
        int id = (start_id + i) % MAX_THREADS;
        if (tcbs[id].state == READY) {
            next_thread = &tcbs[id];
            break;
        }
    }

    if (next_thread == NULL) {
        if (current_thread != &idle_tcb && current_thread->state == RUNNING) {
            return;
        }
        next_thread = &idle_tcb;
    }

    if (next_thread == current_thread) {
        if (current_thread != &idle_tcb) {
            current_thread->state = RUNNING;
        }
        return;
    }

    if (current_thread != &idle_tcb && next_thread != &idle_tcb) {//do not print context switch to/from idle thread
        kprintf("\n");
    }

    if (current_thread != &idle_tcb) {
        current_thread->context = *ctx;
        asm volatile("mrs %0, sp_usr" : "=r"(current_thread->user_sp));
        asm volatile("mrs %0, lr_usr" : "=r"(current_thread->user_lr));

        if (current_thread->state == RUNNING) {
            current_thread->state = READY;
        }
    }

    if (next_thread == &idle_tcb) {//special case when switching to idle thread
        memset(ctx, 0, sizeof(register_context_t));
        ctx->spsr = 0x13;
        ctx->lr = (unsigned int)idle_thread;
    } else {
        *ctx = next_thread->context;
        asm volatile("msr sp_usr, %0" :: "r"(next_thread->user_sp));
        asm volatile("msr lr_usr, %0" :: "r"(next_thread->user_lr));
        next_thread->state = RUNNING;
    }

    current_thread = next_thread;
}

void scheduler_end_current_thread(register_context_t *ctx) {
    if (current_thread != &idle_tcb) {
        current_thread->state = ENDED;
    }
    scheduler_schedule(ctx);
}

void scheduler_enable(void) {
    current_thread = &idle_tcb;
    idle_tcb.state = RUNNING;
    scheduler_enabled = true;
}

struct tcb* scheduler_get_current_thread(void) {
    if (current_thread == &idle_tcb) {
        return NULL;
    }
    return current_thread;
}

void scheduler_wake_waiting_threads(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (tcbs[i].state == WAITING) {
            tcbs[i].state = READY;
        }
    }
}

// Decrement sleep counters and wake threads whose sleep has expired
void scheduler_tick_sleep(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (tcbs[i].state == WAITING && tcbs[i].sleep_ticks > 0) {
            tcbs[i].sleep_ticks--;
            if (tcbs[i].sleep_ticks == 0) {
                // Sleep expired - make thread ready
                tcbs[i].state = READY;
            }
        }
    }
}
