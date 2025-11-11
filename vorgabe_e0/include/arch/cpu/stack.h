#ifndef STACK_H_
#define STACK_H_

#include <arch/cpu/psr.h>

/* Function to read stack pointer from a specific processor mode */
unsigned int read_sp_from_mode(enum psr_mode mode);

/* Verify all stacks are set correctly */
void verify_stacks(void);

#endif // STACK_H_

