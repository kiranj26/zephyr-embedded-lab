/*
 * Lesson 5 — ADC knob -> PWM LED brightness
 *
 * Two demos on the LED (GPIO13, LEDC channel 0). Pick one with DEMO below:
 *   DEMO_KNOB     the potentiometer on GPIO6 (ADC1 channel 5) sets the brightness:
 *                 duty = mV / knob-full-scale-mv (linear), so left = off, right = full
 *   DEMO_BREATHE  go-deeper: the LED fades up and down by itself, with a gamma curve
 *                 so the fade looks even to the eye
 * All hardware details (ADC input, gain, PWM channel, period, pin, calibration) come
 * from the devicetree via the zephyr,user node.
 *
 *   [x] A. ADC: adc_dt_spec from devicetree, setup, read, convert to mV
 *   [x] B. PWM: LED brightness follows the knob (challenge)
 *   [x] Go deeper: breathing LED with gamma correction
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>

/* ---- Demo selection ---------------------------------------------------------------- */
#define DEMO_KNOB    1
#define DEMO_BREATHE 2
#define DEMO         DEMO_KNOB

/* ---- Knob demo ---------------------------------------------------------------------- */

/* Faster than part A's 100 ms so the LED follows the knob without visible lag.
 * The PWM itself runs in hardware at 1 kHz regardless of how often we update it.
 */
static const int32_t sample_interval_ms = 50;

/* Full scale for the mapping, from the overlay (board calibration, not C code):
 * the highest voltage this ADC input reaches. Above it the duty clamps to 100%.
 */
#define KNOB_FULL_SCALE_MV DT_PROP(DT_PATH(zephyr_user), knob_full_scale_mv)

/* Print-on-change threshold, so the console isn't flooded (20 samples/s) */
#define PRINT_STEP_PERCENT 1

/* After this many failed reads in a row, put the LED in a safe state (off) */
#define MAX_CONSECUTIVE_ERRORS 10

/* ---- Breathe demo ------------------------------------------------------------------- */
#define BREATHE_HALF_PERIOD_MS 1000 /* fade up 1 s, fade down 1 s */
#define BREATHE_STEP_MS        10   /* 100 brightness updates per half period */

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
 * The multiply is done in 64 bits: 1000000 ns * 3150 mV = 3.15e9, which overflows
 * 32-bit signed arithmetic. Casting one operand promotes the whole expression.
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

static uint32_t pulse_to_percent(uint32_t pulse_ns, uint32_t period_ns)
{
	return (uint32_t)(((uint64_t)pulse_ns * 100U) / period_ns);
}

/* Last resort when the hardware misbehaves: LED off, so it never sticks at a random level */
static void led_safe_state(void)
{
	(void)pwm_set_pulse_dt(&led, 0);
}

/* DEMO_KNOB: knob -> brightness forever. Only returns if the ADC can't be set up. */
static int run_knob(void)
{
	int ret;
	/* Zephyr ADC samples are signed 16-bit (so differential channels fit); our
	 * calibrated single-ended values are 0..~2934, well within range.
	 */
	int16_t sample;
	int consecutive_errors = 0;
	int32_t last_printed_pct = -100; /* force the first print */

	/*
	 * The request for one conversion. "= { ... }" sets the fields we name and zeroes
	 * the rest (no garbage pointers). adc_sequence_init_dt() then fills in channels,
	 * resolution and oversampling; the buffer is our job.
	 */
	struct adc_sequence sequence = {
		.buffer = &sample,
		.buffer_size = sizeof(sample),
	};

	/* Applies channel@5's settings (gain -> 11 dB attenuation, etc.) to the hardware */
	ret = adc_channel_setup_dt(&knob);
	if (ret < 0) {
		printk("Error %d: could not set up ADC channel %u\n", ret, knob.channel_id);
		return ret;
	}

	/* Once is enough: the sequence describes the same read every time */
	ret = adc_sequence_init_dt(&knob, &sequence);
	if (ret < 0) {
		printk("Error %d: could not init ADC sequence\n", ret);
		return ret;
	}

	printk("Knob demo: %s channel %u, %d-bit, full scale %d mV\n", knob.dev->name,
	       knob.channel_id, knob.resolution, KNOB_FULL_SCALE_MV);

	while (1) {
		k_msleep(sample_interval_ms);

		ret = adc_read_dt(&knob, &sequence);

		/*
		 * Converts in place, so copy first to keep the raw value. With ADC_REF_INTERNAL
		 * it uses the driver's internal reference (1100 mV) and undoes the 1/4 gain.
		 */
		int32_t mv = sample;

		if (ret == 0) {
			ret = adc_raw_to_millivolts_dt(&knob, &mv);
		}

		uint32_t pulse_ns = 0;

		if (ret == 0) {
			pulse_ns = mv_to_pulse_ns(mv, led.period);
			/* Only the duty cycle changes; the period stays at the devicetree's 1 ms */
			ret = pwm_set_pulse_dt(&led, pulse_ns);
		}

		if (ret < 0) {
			consecutive_errors++;
			/* Report the first error, then every 100th, so errors don't flood either */
			if (consecutive_errors == 1 || consecutive_errors % 100 == 0) {
				printk("Error %d (x%d in a row)\n", ret, consecutive_errors);
			}
			/* Returning from main() would freeze the LED at its last duty; go to a
			 * known safe state instead and keep trying.
			 */
			if (consecutive_errors == MAX_CONSECUTIVE_ERRORS) {
				printk("Too many errors: LED off until readings recover\n");
				led_safe_state();
			}
			continue;
		}

		if (consecutive_errors >= MAX_CONSECUTIVE_ERRORS) {
			printk("Recovered after %d errors\n", consecutive_errors);
		}
		consecutive_errors = 0;

		/* Print only when the brightness changes noticeably */
		int32_t pct = (int32_t)pulse_to_percent(pulse_ns, led.period);

		if (pct >= last_printed_pct + PRINT_STEP_PERCENT ||
		    pct <= last_printed_pct - PRINT_STEP_PERCENT) {
			printk("Raw: %4d  mV: %4d  pulse: %7u ns  duty: %3d%%\n", sample, mv,
			       pulse_ns, pct);
			last_printed_pct = pct;
		}
	}
}

/*
 * DEMO_BREATHE: fade up and down forever. Perceived brightness is roughly logarithmic,
 * so a linear duty ramp seems to rush at the bottom and stall at the top. Squaring the
 * fraction (gamma 2) spends more of the ramp in the dim range, so the fade looks even.
 */
static int run_breathe(void)
{
	int ret;
	const uint32_t steps = BREATHE_HALF_PERIOD_MS / BREATHE_STEP_MS;

	printk("Breathe demo: %u ms up, %u ms down, gamma 2\n", BREATHE_HALF_PERIOD_MS,
	       BREATHE_HALF_PERIOD_MS);

	while (1) {
		/* i = 0..steps (fade up), then steps..0 (fade down) */
		for (uint32_t n = 0; n < 2 * steps; n++) {
			uint32_t i = (n < steps) ? n : (2 * steps - n);

			/* pulse = period * (i/steps)^2, in 64 bits: 1e6 * 100 * 100 = 1e10 */
			uint32_t pulse_ns = (uint32_t)(((uint64_t)led.period * i * i) /
						       ((uint64_t)steps * steps));

			ret = pwm_set_pulse_dt(&led, pulse_ns);
			if (ret < 0) {
				printk("Error %d: could not set PWM pulse %u ns\n", ret, pulse_ns);
				led_safe_state();
				return ret;
			}

			k_msleep(BREATHE_STEP_MS);
		}
	}
}

int main(void)
{
	int ret;

	/* Check both drivers before configuring anything (both initialised before main) */
	if (!adc_is_ready_dt(&knob)) {
		printk("Error: ADC device %s not ready\n", knob.dev->name);
		return -ENODEV;
	}

	/* LEDC driver initialised, and pinctrl-0 routed channel 0 to GPIO13 */
	if (!pwm_is_ready_dt(&led)) {
		printk("Error: PWM device %s not ready\n", led.dev->name);
		return -ENODEV;
	}

	printk("LED:  %s channel %u, period %u ns\n", led.dev->name, led.channel, led.period);

	/* A plain if (not #if) so both demos are always compiled and type-checked */
	if (DEMO == DEMO_BREATHE) {
		ret = run_breathe();
	} else {
		ret = run_knob();
	}

	/* The demos loop forever; getting here means setup failed */
	led_safe_state();

	return ret;
}
