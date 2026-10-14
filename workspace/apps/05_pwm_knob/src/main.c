/*
 * Lesson 5 — ADC knob -> PWM LED brightness
 *   A1. Get the ADC device + channel config from DT (DEVICE_DT_GET, ADC_CHANNEL_CFG_DT)
 *   A2. adc_channel_setup(), build a struct adc_sequence (channels, buffer, resolution)
 *   A3. Loop: adc_read() -> print raw and millivolts (vref from DT_PROP(..., zephyr_vref_mv))
 *   B1. Get a pwm_dt_spec (PWM_DT_SPEC_GET(DT_ALIAS(led_0)))
 *   B2. pulse = period * raw / 2^resolution ; pwm_set_dt()
 * Go deeper: replace your mV math with adc_raw_to_millivolts_dt(); check it with a multimeter.
 */
#include <zephyr/kernel.h>
/* #include <zephyr/drivers/adc.h> */
/* #include <zephyr/drivers/pwm.h> */

int main(void)
{
	printk("Lesson 5 starter\r\n");
	while (1) {
		/* TODO */
		k_msleep(100);
	}
	return 0;
}
