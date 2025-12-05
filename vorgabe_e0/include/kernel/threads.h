#pragma once

#include <arch/cpu/registers.h>

#define MAX_THREADS 32
#define THREAD_STACK_SIZE 0x800  // 2 KiB per thread

/* Linker script symbols for thread stack region */
extern char _thread_stacks_start[];
extern char _thread_stacks_end[];

enum thread_state {
    FREE,     // Thread slot is unused
    RUNNING,
    READY,
    WAITING,
    ENDED
};

struct tcb {
    // Thread Control Block structure definition
    int thread_id;
    register_context_t context;
    enum thread_state state;
    void* stack_base;  // Pointer to bottom of this thread's stack
    // User mode banked registers (saved/restored separately)
    unsigned int user_sp;
    unsigned int user_lr;
};

extern struct tcb tcbs[MAX_THREADS];

// Thread system initialization
void threads_init(void);

// Thread creation function (as specified in assignment)
void scheduler_thread_create(void (*func)(void*), const void *arg, unsigned int arg_size);

/* Helper macro to get stack top for thread i */
#define THREAD_STACK_TOP(i) ((void*)(_thread_stacks_end - (i) * THREAD_STACK_SIZE))
