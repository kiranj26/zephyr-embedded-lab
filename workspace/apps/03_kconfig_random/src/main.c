/*
 * Lesson 3 — Kconfig
 *
 * Every second: print a random 32-bit number, 5 random bytes and the same number
 * as a float in [0, 1), then (if the greeting module is enabled) greet
 * CONFIG_GREETING_REPEAT times.
 *
 *   [x] 1. sys_rand32_get() / sys_rand_get(): link error until a random generator is
 *          enabled. Fixed with CONFIG_ENTROPY_GENERATOR=y (hardware TRNG) in
 *          boards/esp32s3_devkitc.conf
 *   [x] 2. Challenge: "%.3f" printed "*float*" until CONFIG_CBPRINTF_FP_SUPPORT=y
 *          (prj.conf). The FPU is NOT needed for printing; see lessons/03_kconfig/notes.md
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

		/*
		 * Scale to [0, 1): divide by 2^32 (not UINT32_MAX, which could give exactly 1.0).
		 * The double math runs in software (libgcc routines in the ESP32-S3 ROM);
		 * printing it needs CONFIG_CBPRINTF_FP_SUPPORT, otherwise "%.3f" prints "*float*".
		 */
		double f = (double)rnd / (UINT32_MAX + 1.0);

		printk("Random float: %.3f\n", f);

#ifdef CONFIG_GREETING
		greeting_say();
#endif
		k_msleep(1000);
	}

	return 0;
}
