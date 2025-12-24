#pragma once

#include <arch/cpu/registers.h>

#define MAX_THREADS 32
#define THREAD_STACK_SIZE 0x800

extern char _thread_stacks_start[];
extern char _thread_stacks_end[];

enum thread_state {
    FREE,
    RUNNING,
    READY,
    WAITING,
    ENDED
};

struct tcb {
    int thread_id;
    register_context_t context;
    enum thread_state state;
    void* stack_base;
    unsigned int user_sp;
    unsigned int user_lr;
    unsigned int sleep_ticks;  // Number of timer ticks remaining to sleep
};

extern struct tcb tcbs[MAX_THREADS];

void threads_init(void);
void scheduler_thread_create(void (*func)(void*), const void *arg, unsigned int arg_size);

#define THREAD_STACK_TOP(i) ((void*)(_thread_stacks_end - (i) * THREAD_STACK_SIZE))
