#ifndef TIMER_H_
#define TIMER_H_

#include <stddef.h>
#include <assert.h>

static const unsigned int TIMER_BASE = 0x7E003000 - 0x3F000000;

struct timer {
	unsigned int CS;
	unsigned int CLO;
	unsigned int CHI;
	unsigned int unused1;
	unsigned int C1;
	unsigned int unused2;
	unsigned int C3;
};

static_assert(offsetof(struct timer, CS) == 0x00);
static_assert(offsetof(struct timer, CLO) == 0x04);
static_assert(offsetof(struct timer, CHI) == 0x08);
static_assert(offsetof(struct timer, C1) == 0x10);
static_assert(offsetof(struct timer, C3) == 0x18);

static volatile struct timer *const timer_instance = (struct timer *)TIMER_BASE;


#define TIMER_STATUS_1 (1 << 1)  //C1 compare
#define TIMER_STATUS_3 (1 << 3)  //C3 compare
void system_timer_init(void);
void system_timer_interrupt_handler(void);

#endif // TIMER_H_