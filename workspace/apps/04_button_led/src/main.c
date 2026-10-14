/*
 * Lesson 4 — Devicetree: the button controls the LED.
 *   1. Write the overlay (aliases my-led, my-button)
 *   2. Get gpio_dt_spec for both (GPIO_DT_SPEC_GET + DT_ALIAS)
 *   3. Check readiness, configure LED as output and button as input
 *   4. Loop every 100 ms: read button (gpio_pin_get_dt) -> set LED
 *   5. Print btn.dt_flags in hex. Decode the bits using include/zephyr/dt-bindings/gpio/gpio.h
 * Self-check: what does gpio_pin_get_dt return when pressed, and WHY (logical vs physical)?
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* TODO: static const struct gpio_dt_spec led = ...; */
/* TODO: static const struct gpio_dt_spec btn = ...; */

int main(void)
{
	printk("Lesson 4 starter — write the overlay first\r\n");

	while (1) {
		/* TODO */
		k_msleep(100);
	}
	return 0;
}
