#ifndef EXCEPTION_PRINT_H_
#define EXCEPTION_PRINT_H_

#include "arch/cpu/registers.h"
#include <stdbool.h>

/* Print PSR (Program Status Register) flags and mode */
void print_psr(unsigned int psr);

/* Get mode name string from mode value */
char *get_mode_name(unsigned int mode);

/* Read mode-specific registers (SP, LR, CPSR/SPSR) for all modes */
mode_regs_t read_mode_specific_registers(void);

/* Print detailed exception information */
void print_exception_infos(register_context_t* ctx, bool is_data_abort, bool is_prefetch_abort, char* exception_name, unsigned int exception_source_addr);

#endif // EXCEPTION_PRINT_H_

