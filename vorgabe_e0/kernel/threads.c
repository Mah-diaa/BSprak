#include <kernel/threads.h>
#include <lib/kprintf.h>
#include <lib/string.h>

struct tcb tcbs[MAX_THREADS];

//attributes to shut up warnings
extern void syscall_exit(void) __attribute__((noreturn));
extern struct tcb* scheduler_get_current_thread(void);

static void thread_wrapper(void) __attribute__((noreturn));
static void thread_wrapper(void) {
    syscall_exit();
}

void threads_init(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        tcbs[i].state = FREE;
        tcbs[i].thread_id = -1;
    }
}

static int find_free_thread_slot(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (tcbs[i].state == FREE) {
            return i;
        }
    }
    return -1;
}

void scheduler_thread_create(void (*func)(void*), const void *arg, unsigned int arg_size) {
    int tid = find_free_thread_slot();
    if (tid < 0) {//max threads
        kprintf("Could not create thread\n");
        return;
    }

    struct tcb *tcb = &tcbs[tid];
    tcb->thread_id = tid;
    tcb->state = READY;
    tcb->stack_base = (void*)(_thread_stacks_end - (tid + 1) * THREAD_STACK_SIZE);//top of stack is the end of the next thread's stack

    register_context_t *context = &tcb->context;
    memset(context, 0, sizeof(register_context_t));

    unsigned int stack_pointer = (unsigned int)THREAD_STACK_TOP(tid);

    if (arg != NULL && arg_size > 0) {
        unsigned int aligned_size = (arg_size + 7) & ~7;
        stack_pointer -= aligned_size;
        memcpy((void *)stack_pointer, arg, arg_size);
    }

    //thread initialization
    context->lr = (unsigned int)func;
    tcb->user_sp = stack_pointer;
    tcb->user_lr = (unsigned int)thread_wrapper;
    context->r0 = stack_pointer;
    context->spsr = 0x10;
}
