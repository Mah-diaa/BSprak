#ifndef TIMER_H_
#define TIMER_H_

#include <stddef.h>
#include <assert.h>

static const unsigned int TIMER_BASE = 0x7E003000 - 0x3F000000;

struct timer {
	unsigned int CS;   /* Control/Status register at offset 0x00 */
	unsigned int CLO;  /* Counter Lower 32 bits at offset 0x04 */
	unsigned int CHI;  /* Counter Higher 32 bits at offset 0x08 */
	unsigned int unused1;  /* C0 at offset 0x0C - not used */
	unsigned int C1;   /* Compare 1 at offset 0x10 */
	unsigned int unused2;  /* C2 at offset 0x14 - not used */
	unsigned int C3;   /* Compare 3 at offset 0x18 */
};

static_assert(offsetof(struct timer, CS) == 0x00);
static_assert(offsetof(struct timer, CLO) == 0x04);
static_assert(offsetof(struct timer, CHI) == 0x08);
static_assert(offsetof(struct timer, C1) == 0x10);
static_assert(offsetof(struct timer, C3) == 0x18);

static volatile struct timer *const timer_instance = (struct timer *)TIMER_BASE;

/* Timer status bits in CS register */
#define TIMER_STATUS_1 (1 << 1)  /* Status bit for C1 compare match */
#define TIMER_STATUS_3 (1 << 3)  /* Status bit for C3 compare match */

void system_timer_init(void);
void system_timer_interrupt_handler(void);

#endif // TIMER_H_

