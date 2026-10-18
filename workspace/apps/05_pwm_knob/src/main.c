/*
 * Lesson 5 — ADC knob -> PWM LED brightness
 *
 * Every 50 ms: read the potentiometer on GPIO6 (ADC1 channel 5), convert it to
 * millivolts, and set the LED's PWM duty cycle to the same fraction of 3.3 V
 * (linear mapping). Knob left = off, middle = ~half duty, right = ~full.
 * All hardware details (ADC input, gain, PWM channel, period, pin) come from the
 * devicetree via the zephyr,user node.
 *
 *   [x] A. ADC: adc_dt_spec from devicetree, setup, read, convert to mV
 *   [x] B. PWM: LED brightness follows the knob (challenge)
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>

/* Faster than part A's 100 ms so the LED follows the knob without visible lag.
 * The PWM itself runs in hardware at 1 kHz regardless of how often we update it.
 */
static const int32_t sample_interval_ms = 50;

/* Full scale for the mapping: the supply voltage. The knob tops out at ~3151 mV
 * (ESP32-S3 at 11 dB attenuation), so full right gives ~95% duty.
 */
#define KNOB_FULL_SCALE_MV 3300

/*
 * Follows zephyr,user's io-channels entry named "knob" -> <&adc0 5> and fills in
 * .dev (adc0), .channel_id (5) and .channel_cfg (gain, reference, ...) from channel@5.
 */
static const struct adc_dt_spec knob = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), knob);

/*
 * Follows zephyr,user's pwms entry named "led" -> <&ledc0 0 PWM_HZ(1000) ...> and fills in
 * .dev (ledc0), .channel (0), .period (1000000 ns) and .flags (normal polarity).
 */
static const struct pwm_dt_spec led = PWM_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), led);

/*
 * Linear map: pulse = period * mv / full_scale, clamped to [0, period].
 * The multiply is done in 64 bits: 1000000 ns * 3300 mV = 3.3e9, which overflows
 * 32-bit arithmetic. Casting one operand promotes the whole expression.
 */
static uint32_t mv_to_pulse_ns(int32_t mv, uint32_t period_ns)
{
	if (mv <= 0) {
		return 0;
	}
	if (mv >= KNOB_FULL_SCALE_MV) {
		return period_ns;
	}

	return (uint32_t)(((uint64_t)period_ns * (uint32_t)mv) / KNOB_FULL_SCALE_MV);
}

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

	/* LEDC driver initialised (pins routed by pinctrl-0) before main() */
	if (!pwm_is_ready_dt(&led)) {
		printk("Error: PWM device %s not ready\n", led.dev->name);
		return -ENODEV;
	}

	printk("Knob: %s channel %d, %d-bit\n", knob.dev->name, knob.channel_id,
	       knob.resolution);
	printk("LED:  %s channel %u, period %u ns\n", led.dev->name, led.channel, led.period);

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
			printk("Error %d: mV conversion failed (raw %d)\n", ret, sample);
			k_msleep(sample_interval_ms);
			continue;
		}

		uint32_t pulse_ns = mv_to_pulse_ns(mv, led.period);

		/* Only the duty cycle changes; the period stays at the devicetree's 1 ms */
		ret = pwm_set_pulse_dt(&led, pulse_ns);
		if (ret < 0) {
			printk("Error %d: could not set PWM pulse %u ns\n", ret, pulse_ns);
			return ret;
		}

		printk("Raw: %4d  mV: %4d  pulse: %7u ns  duty: %3u%%\n", sample, mv, pulse_ns,
		       (uint32_t)(((uint64_t)pulse_ns * 100U) / led.period));

		k_msleep(sample_interval_ms);
	}

	return 0;
}
