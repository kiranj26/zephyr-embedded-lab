/*
 * Lesson 2 — CMake: blink + a library that says hello.
 * Steps:
 *   1. Finish lib/say_hello/CMakeLists.txt (library target)
 *   2. Wire it into ../CMakeLists.txt (add_subdirectory + target_link_libraries)
 *   3. Uncomment the include + call below, build, flash, capture.
 * Explore: open workspace/build/02_hello_lib/build.ninja and find your library.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
/* #include "say_hello.h" */

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

int main(void)
{
	int state = 0;

	if (!gpio_is_ready_dt(&led) || gpio_pin_configure_dt(&led, GPIO_OUTPUT) < 0) {
		printk("LED setup failed\r\n");
		return 0;
	}

	while (1) {
		state = !state;
		gpio_pin_set_dt(&led, state);
		/* say_hello(); */
		k_msleep(1000);
	}
	return 0;
}
