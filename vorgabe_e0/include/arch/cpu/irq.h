#pragma once

/**
 * \file irq.h
 *
 * IRQ (interrupt) control functions
 */

/**
 * Disable IRQs and return previous state
 * @return Previous IRQ state (true if were enabled, false if disabled)
 */
static inline bool irq_disable(void) {
	unsigned int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));
	bool was_enabled = !(cpsr & (1 << 7));  // I bit is 0 when enabled
	asm volatile("cpsid i");  // Disable IRQs
	return was_enabled;
}

/**
 * Enable IRQs
 */
static inline void irq_enable(void) {
	asm volatile("cpsie i");  // Enable IRQs
}

/**
 * Restore IRQ state
 * @param was_enabled Previous state from irq_disable()
 */
static inline void irq_restore(bool was_enabled) {
	if (was_enabled) {
		irq_enable();
	}
}
