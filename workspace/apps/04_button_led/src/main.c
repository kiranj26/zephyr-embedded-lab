/*
 * Lesson 4 — Devicetree: the button controls the LED.
 *
 * Polls the button on GPIO5 every 100 ms and mirrors it on the LED (GPIO13):
 * pressed = on, released = off. Both pins come from the devicetree overlay
 * (boards/esp32s3_devkitc.overlay) through the aliases my-button and my-led.
 *
 * Logical vs physical: the button is wired GPIO5 -> switch -> GND with
 * (GPIO_ACTIVE_LOW | GPIO_PULL_UP). Pressed pulls the pin to 0 V, but because it's
 * declared active-low, gpio_pin_get_dt() returns 1 ("active") when pressed.
 *
 * Polling, not interrupts: simple, but up to 100 ms latency and 10 wake-ups/s.
 * Lesson 09 replaces this loop with a GPIO interrupt.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/*
 * Also acts as the debounce: a mechanical switch bounces for ~1-10 ms, far shorter
 * than one poll period, so each read sees a settled level. Shorten this (or switch
 * to interrupts) and bounces would show up as extra pressed/released events.
 */
static const int32_t poll_interval_ms = 100;

/* .port = gpio0 controller, .pin = 5, .dt_flags = 0x11 (ACTIVE_LOW | PULL_UP) */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);

/* .port = gpio0 controller, .pin = 13, .dt_flags = 0x0 (ACTIVE_HIGH) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

int main(void)
{
	int ret;
	int last_state = -1;

	if (!gpio_is_ready_dt(&button)) {
		printk("Error: button GPIO controller not ready\n");
		return -ENODEV;
	}

	if (!gpio_is_ready_dt(&led)) {
		printk("Error: LED GPIO controller not ready\n");
		return -ENODEV;
	}

	/* Output, starting in the logical "off" state */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: could not configure LED\n", ret);
		return ret;
	}

	/* Input only: the pull-up comes from .dt_flags, which configure_dt merges in */
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		printk("Error %d: could not configure button\n", ret);
		return ret;
	}

	/* Decode the devicetree flags (values from dt-bindings/gpio/gpio.h) */
	printk("Button: pin %u, dt_flags 0x%02x (%s%s)\n", button.pin, button.dt_flags,
	       (button.dt_flags & GPIO_ACTIVE_LOW) ? "ACTIVE_LOW" : "ACTIVE_HIGH",
	       (button.dt_flags & GPIO_PULL_UP) ? " | PULL_UP" : "");
	printk("LED:    pin %u, dt_flags 0x%02x\n", led.pin, led.dt_flags);

	while (1) {
		/* Logical level: 1 = pressed (active-low already applied) */
		int pressed = gpio_pin_get_dt(&button);

		if (pressed < 0) {
			printk("Error %d: could not read button\n", pressed);
			return pressed;
		}

		/* Logical level: 1 = LED on */
		ret = gpio_pin_set_dt(&led, pressed);
		if (ret < 0) {
			printk("Error %d: could not set LED\n", ret);
			return ret;
		}

		/* Print only on changes, so the console stays readable */
		if (pressed != last_state) {
			printk("Button %s\n", pressed ? "pressed  -> LED on" : "released -> LED off");
			last_state = pressed;
		}

		k_msleep(poll_interval_ms);
	}

	return 0;
}
