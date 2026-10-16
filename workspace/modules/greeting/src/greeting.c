#include <zephyr/kernel.h>
#include "greeting.h"

void greeting_say(void)
{
	for (int i = 0; i < CONFIG_GREETING_REPEAT; i++) {
		printk("Hello from the greeting module! (%d/%d)\n", i + 1, CONFIG_GREETING_REPEAT);
	}
}
