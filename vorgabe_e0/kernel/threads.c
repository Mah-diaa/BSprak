#include <kernel/threads.h>
#include <lib/kprintf.h>
#include <lib/string.h>

// Define the global thread control block array
struct tcb tcbs[MAX_THREADS];

// Forward declaration for syscall_exit (will be implemented later)
extern void syscall_exit(void) __attribute__((noreturn));

// Get current thread from scheduler
extern struct tcb* scheduler_get_current_thread(void);

// Simple exit handler - called when thread function returns
// This is set as user_lr, so when func does 'bx lr', it jumps here
static void thread_wrapper(void) __attribute__((noreturn));
static void thread_wrapper(void) {
    // Thread function returned, trigger SVC to exit
    syscall_exit();
}

// Initialize thread system must be called before creating threads
void threads_init(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        tcbs[i].state = FREE;
        tcbs[i].thread_id = -1;
    }
}

// Find a free thread slot
static int find_free_thread_slot(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (tcbs[i].state == FREE) {
            return i;
        }
    }
    return -1; 
}

void scheduler_thread_create(void (*func)(void*), const void *arg, unsigned int arg_size) {
    // Find free thread slot
    int tid = find_free_thread_slot();
    if (tid < 0) {
        kprintf("Could not create thread\n");
        return;
    }

    struct tcb *tcb = &tcbs[tid];
    tcb->thread_id = tid;
    tcb->state = READY;
    tcb->stack_base = (void*)(_thread_stacks_end - (tid + 1) * THREAD_STACK_SIZE);

    // Zero out context (like friend's code)
    register_context_t *context = &tcb->context;
    memset(context, 0, sizeof(register_context_t));

    // Set up stack pointer at top of thread's stack
    unsigned int stack_pointer = (unsigned int)THREAD_STACK_TOP(tid);

    // Copy argument to stack if provided (with 8-byte alignment)
    if (arg != NULL && arg_size > 0) {
        // Align arg_size to 8 bytes (required by ARM EABI)
        unsigned int aligned_size = (arg_size + 7) & ~7;
        stack_pointer -= aligned_size;
        memcpy((void *)stack_pointer, arg, arg_size);
    }

    // Thread initialization matching friend's approach:
    context->lr = (unsigned int)func;           // Exception LR = function to run
    tcb->user_sp = stack_pointer;               // User mode stack pointer
    tcb->user_lr = (unsigned int)thread_wrapper; // User mode LR = exit handler
    context->r0 = stack_pointer;                // r0 = pointer to args on stack
    context->spsr = 0x10;                       // User mode (0b10000)

}
