/*
 * Lesson 2 — CMake: blink + a library that says hello.
 *
 * Toggles the LED on GPIO13 once a second and calls say_hello() from the
 * lib/say_hello static library each time.
 *
 * Build pieces: lib/say_hello/CMakeLists.txt creates the say_hello target,
 * ../CMakeLists.txt links it into Zephyr's app target.
 * Explore: workspace/build/02_hello_lib/build.ninja (libsay_hello.a) and zephyr.map.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "say_hello.h"

static const int32_t sleep_time_ms = 1000;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

int main(void)
{
	int ret;
	int state = 0;

	/* Separate checks so a failure says which step went wrong */
	if (!gpio_is_ready_dt(&led)) {
		printk("Error: LED GPIO not ready\n");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: could not configure LED\n", ret);
		return ret;
	}

	while (1) {
		state = !state;

		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			printk("Error %d: could not set LED\n", ret);
			return ret;
		}

		say_hello();

		k_msleep(sleep_time_ms);
	}

	return 0;
}
