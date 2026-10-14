/*
 * Lesson 6 — Device drivers
 * Part A (demo): understand course/workspace/modules/button — trace one call from
 *   btn_api->get() in the app down to gpio_pin_get_dt() in the driver.
 * Part B (challenge): your own MCP9808 driver module (start from modules/_templates/sensor_driver)
 *   and the workshop: github.com/ShawnHymel/workshop-zephyr-device-driver
 *
 * App TODOs:
 *   1. const struct device *const sensor = DEVICE_DT_GET(DT_ALIAS(my_sensor));
 *   2. device_is_ready(sensor)
 *   3. loop: sensor_sample_fetch(); sensor_channel_get(SENSOR_CHAN_AMBIENT_TEMP, &val);
 *            printk("Temperature: %d.%06d\n", val.val1, val.val2);
 */
#include <zephyr/kernel.h>
/* #include <zephyr/drivers/sensor.h> */

int main(void)
{
	printk("Lesson 6 starter\r\n");
	while (1) {
		k_msleep(1000);
	}
	return 0;
}
