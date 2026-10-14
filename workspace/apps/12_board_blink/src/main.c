/*
 * Lesson 12 — no overlay! The LED (alias led-0) and button (sw0) are part of the board definition
 * in workspace/boards/custom/s3_n16r8/s3_n16r8_procpu.dts.
 * Challenge: add the ST7735R display + I2C sensor to YOUR board, then run the lesson-10 UI
 * with BOARD=s3_n16r8/esp32s3/procpu and zero overlays.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led_0), gpios);

int main(void)
{
	int state = 0;

	if (!gpio_is_ready_dt(&led) || gpio_pin_configure_dt(&led, GPIO_OUTPUT) < 0) {
		printk("LED setup failed\r\n");
		return 0;
	}
	printk("Hello from board: %s\r\n", CONFIG_BOARD);

	while (1) {
		state = !state;
		gpio_pin_set_dt(&led, state);
		k_msleep(500);
	}
	return 0;
}
