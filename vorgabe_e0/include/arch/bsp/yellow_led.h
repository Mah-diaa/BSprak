#ifndef YELLOW_LED_H
#define YELLOW_LED_H
#include <stddef.h>
#include <assert.h>



static const unsigned int GPIO_BASE		   = 0x7E200000 - 0x3F000000;
static const unsigned int YELLOW_LED	   = 7u;
static const unsigned int GPF_BITS		   = 3u;
static const unsigned int GPF_MASK		   = 0x7u;
static const unsigned int YELLOW_LED_GPF_SHIFT = YELLOW_LED * GPF_BITS;

enum gpio_func {
	gpio_input  = 0x0,
	gpio_output = 0x1,
};

struct gpio {
	unsigned int func[6];
	unsigned int unused0;
	unsigned int set[2];
	unsigned int unused1;
	unsigned int clr[2];
};

static_assert(offsetof(struct gpio, func) == 0x00);
static_assert(offsetof(struct gpio, set) == 0x1C);
static_assert(offsetof(struct gpio, clr) == 0x28);

static volatile struct gpio *const gpio_port = (struct gpio *)GPIO_BASE;




void init_yellow(void);
void yellow_on(void);
void yellow_off(void);

#endif
