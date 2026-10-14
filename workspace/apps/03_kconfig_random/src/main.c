/*
 * Lesson 3 — Kconfig
 *   1. Print a random 32-bit number every second with sys_rand32_get()
 *      -> Build fails? Good. Find the missing CONFIG_ via menuconfig.
 *   2. Challenge: print it as a float in [0,1) with "%.3f". What Kconfig is needed?
 *   3. Create your own module with a CONFIG_GREETING bool that compiles in greeting_say().
 */
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#ifdef CONFIG_GREETING
/* #include "greeting.h" */
#endif

int main(void)
{
	while (1) {
		/* TODO 1: uint32_t rnd = sys_rand32_get(); printk(...) */
		/* TODO 2: double f = (double)rnd / (UINT32_MAX + 1.0); printk("%.3f\r\n", f); */
#ifdef CONFIG_GREETING
		/* TODO 3: greeting_say(); */
#endif
		k_msleep(1000);
	}
	return 0;
}
