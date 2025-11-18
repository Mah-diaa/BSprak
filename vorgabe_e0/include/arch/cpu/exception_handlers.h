#ifndef EXCEPTION_HANDLERS_H_
#define EXCEPTION_HANDLERS_H_

#include "arch/cpu/registers.h"

void handle_supervisor_call_trampoline(register_context_t* ctx);
void handle_undefined_instruction_trampoline(register_context_t* ctx);
void handle_prefetch_abort_trampoline(register_context_t* ctx);
void handle_data_abort_trampoline(register_context_t* ctx);
void handle_not_used_trampoline(register_context_t* ctx);
void handle_irq_trampoline(register_context_t* ctx);
void handle_fiq_trampoline(register_context_t* ctx);

#endif // EXCEPTION_HANDLERS_H_
