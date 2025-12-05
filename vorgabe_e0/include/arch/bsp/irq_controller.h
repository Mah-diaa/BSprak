#ifndef IRQ_CONTROLLER_H_
#define IRQ_CONTROLLER_H_

#include <stddef.h>
#include <assert.h>

static const unsigned int IRQ_CONTROLLER_BASE = 0x7E00B000 - 0x3F000000;

struct irq_controller {
	unsigned int unused[128];      // Registers 0x000-0x1FC (128 * 4 = 0x200)
	unsigned int IRQ_basic_pending;
	unsigned int PENDING_1;
	unsigned int PENDING_2;
	unsigned int FIQ_control;
	unsigned int ENABLE_1;
	unsigned int ENABLE_2;
};

static_assert(offsetof(struct irq_controller, IRQ_basic_pending) == 0x200);
static_assert(offsetof(struct irq_controller, PENDING_1) == 0x204);
static_assert(offsetof(struct irq_controller, PENDING_2) == 0x208);
static_assert(offsetof(struct irq_controller, FIQ_control) == 0x20C);
static_assert(offsetof(struct irq_controller, ENABLE_1) == 0x210);
static_assert(offsetof(struct irq_controller, ENABLE_2) == 0x214);

static volatile struct irq_controller *const irq_controller_instance =
	(struct irq_controller *)IRQ_CONTROLLER_BASE;

#define TIMER_INTERRUPT_MASK (1 << 1)
#define UART_INTERRUPT_MASK (1 << 25)

struct register_context;  // Forward declaration

void irq_controller_init(void);
void irq_controller_handler(struct register_context *ctx);

#endif // IRQ_CONTROLLER_H_