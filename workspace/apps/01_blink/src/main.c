/*
 * Lesson 1 — Getting Started: Blink
 * Goal: retype this from understanding, not copy. Every line should make sense to you.
 *
 * Extension TODOs (after it works):
 *   [ ] Blink "SOS" in Morse (... --- ...)
 *   [ ] Add a second LED in the overlay (pick a safe pin!) and alternate them
 *   [ ] Print k_uptime_get() each toggle — how accurate is k_msleep?
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const int32_t sleep_time_ms = 1000;

/* Q: Where does DT_ALIAS(my_led) come from? (hint: boards/esp32s3_devkitc.overlay) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

int main(void)
{
	int ret;
	int state = 0;

	if (!gpio_is_ready_dt(&led)) {
		printk("Error: LED GPIO not ready\r\n");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		printk("Error %d: could not configure LED\r\n", ret);
		return 0;
	}

	while (1) {
		state = !state;
		printk("LED state: %d\r\n", state);

		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			printk("Error %d: could not set LED\r\n", ret);
			return 0;
		}

		k_msleep(sleep_time_ms);
	}

	return 0;
}
