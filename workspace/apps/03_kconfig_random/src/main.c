/*
 * Lesson 3 — Kconfig
 *
 * Every second: print a random 32-bit number and 5 random bytes, then (if the
 * greeting module is enabled) greet CONFIG_GREETING_REPEAT times.
 *
 *   [x] 1. sys_rand32_get() / sys_rand_get(): link error until a random generator is
 *          enabled. Fixed with CONFIG_ENTROPY_GENERATOR=y (hardware TRNG) in
 *          boards/esp32s3_devkitc.conf; see lessons/03_kconfig/notes.md
 *   [ ] 2. Challenge: print it as a float in [0,1) with "%.3f". What Kconfig is needed?
 *   [x] 3. Own module (workspace/modules/greeting) with CONFIG_GREETING (bool) and
 *          CONFIG_GREETING_REPEAT (int, 1..10), enabled in prj.conf
 */
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#ifdef CONFIG_GREETING
#include "greeting.h"
#endif

#define NUM_RANDOM_BYTES 5

int main(void)
{
	uint8_t buffer[NUM_RANDOM_BYTES];

	while (1) {
		/* One 32-bit value from the configured random generator */
		uint32_t rnd = sys_rand32_get();

		printk("Random u32:   %u\n", rnd);

		/* Fill a buffer with random bytes (same generator) */
		sys_rand_get(buffer, sizeof(buffer));

		printk("Random bytes:");
		for (int i = 0; i < NUM_RANDOM_BYTES; i++) {
			printk(" %3u", buffer[i]);
		}
		printk("\n");

		/* TODO 2 (challenge): print rnd as a float in [0, 1) with "%.3f"
		 * double f = (double)rnd / (UINT32_MAX + 1.0);
		 * printk("Random float: %.3f\n", f);
		 */

#ifdef CONFIG_GREETING
		greeting_say();
#endif
		k_msleep(1000);
	}

	return 0;
}
