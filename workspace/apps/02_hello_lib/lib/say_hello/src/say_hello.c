/* Own header first: proves say_hello.h compiles on its own */
#include "say_hello.h"

#include <zephyr/sys/printk.h>

void say_hello(void)
{
	/* printk, not printf: Zephyr's lightweight console print, no libc (picolibc) stdio */
	printk("Hello!!\n");
}
