#ifndef EXCEPTION_PRINT_H_
#define EXCEPTION_PRINT_H_

#include "arch/cpu/registers.h"
#include <stdbool.h>

void print_psr(unsigned int psr);

char *get_mode_name(unsigned int mode);

mode_regs_t read_mode_specific_registers(register_context_t* ctx);

void print_exception_infos(register_context_t* ctx, bool is_data_abort, bool is_prefetch_abort, char* exception_name, unsigned int exception_source_addr);

#endif // EXCEPTION_PRINT_H_

