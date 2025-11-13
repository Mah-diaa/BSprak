#ifndef EXCEPTION_TRIGGERS_H_
#define EXCEPTION_TRIGGERS_H_

/* Trigger a Supervisor Call (SVC) exception */
void do_supervisor_call(void);

/* Trigger an Undefined Instruction exception */
void do_undefined_inst(void);

/* Trigger a Data Abort exception */
void do_data_abort(void);

/* Trigger a Prefetch Abort exception */
void do_prefetch_abort(void);

#endif // EXCEPTION_TRIGGERS_H_

