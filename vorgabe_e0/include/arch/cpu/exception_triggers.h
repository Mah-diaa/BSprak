#ifndef EXCEPTION_TRIGGERS_H_
#define EXCEPTION_TRIGGERS_H_

void do_supervisor_call(void);

void do_undefined_inst(void);

void do_data_abort(void);

void do_prefetch_abort(void);

#endif // EXCEPTION_TRIGGERS_H_

