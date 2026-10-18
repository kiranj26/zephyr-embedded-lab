/*
 * Lesson 5 — ADC knob (-> PWM LED brightness in part B)
 *
 * Part A: read the potentiometer on GPIO6 (ADC1 channel 5) every 100 ms and print
 * the value and the voltage. Everything about the channel (which ADC, which input,
 * gain, reference, resolution) comes from the devicetree via the zephyr,user node.
 *
 *   [x] A. ADC: adc_dt_spec from devicetree, setup, read, convert to mV
 *   [ ] B. PWM: LED brightness follows the knob (challenge)
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

static const int32_t sample_interval_ms = 100;

/*
 * Follows zephyr,user's io-channels entry named "knob" -> <&adc0 5> and fills in
 * .dev (adc0), .channel_id (5) and .channel_cfg (gain, reference, ...) from channel@5.
 */
static const struct adc_dt_spec knob = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), knob);

int main(void)
{
	int ret;
	int16_t sample;

	/*
	 * The request for one conversion. "= { ... }" sets the fields we name and zeroes
	 * the rest (no garbage pointers). adc_sequence_init_dt() then fills in channels,
	 * resolution and oversampling; the buffer is our job.
	 */
	struct adc_sequence sequence = {
		.buffer = &sample,
		.buffer_size = sizeof(sample),
	};

	if (!adc_is_ready_dt(&knob)) {
		printk("Error: ADC device %s not ready\n", knob.dev->name);
		return -ENODEV;
	}

	/* Applies channel@5's settings (gain -> 11 dB attenuation, etc.) to the hardware */
	ret = adc_channel_setup_dt(&knob);
	if (ret < 0) {
		printk("Error %d: could not set up ADC channel %d\n", ret, knob.channel_id);
		return ret;
	}

	/* Once is enough: the sequence describes the same read every time */
	ret = adc_sequence_init_dt(&knob, &sequence);
	if (ret < 0) {
		printk("Error %d: could not init ADC sequence\n", ret);
		return ret;
	}

	printk("Knob on %s channel %d, %d-bit\n", knob.dev->name, knob.channel_id,
	       knob.resolution);

	while (1) {
		ret = adc_read_dt(&knob, &sequence);
		if (ret < 0) {
			printk("Error %d: ADC read failed\n", ret);
			k_msleep(sample_interval_ms);
			continue;
		}

		/*
		 * Converts in place, so copy first to keep the raw value. With ADC_REF_INTERNAL
		 * it uses the driver's internal reference (1100 mV) and undoes the 1/4 gain.
		 */
		int32_t mv = sample;

		ret = adc_raw_to_millivolts_dt(&knob, &mv);
		if (ret < 0) {
			printk("Raw: %4d  (mV conversion failed: %d)\n", sample, ret);
		} else {
			printk("Raw: %4d  mV: %4d\n", sample, mv);
		}

		k_msleep(sample_interval_ms);
	}

	return 0;
}
