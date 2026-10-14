/*
 * Lesson 1 — Getting Started: Blink
 * Goal: retype this from understanding, not copy. Every line should make sense to you.
 *
 * Three demos on the LEDs at GPIO13 (LED 1) and GPIO17 (LED 2). Pick one with DEMO below:
 *   DEMO_TIMED_BLINK  1 Hz blink that prints how long each toggle and sleep really took
 *   DEMO_SOS          "SOS" in Morse code, printing the measured length of each cycle
 *   DEMO_ALTERNATE    LED 1 and LED 2 always opposite, swapping once a second
 *
 * Extension TODOs (after it works):
 *   [x] Blink "SOS" in Morse (... --- ...)            → DEMO_SOS
 *       Result: cycle = 6802.2 ms vs 6800 ms ideal, i.e. +1 tick for each of the 22 sleeps
 *   [x] Add a second LED in the overlay (pick a safe pin!) and alternate them → DEMO_ALTERNATE
 *       GPIO17, chosen with lessons/01_getting-started/gpio-analysis.md
 *   [x] Print k_uptime_get() each toggle — how accurate is k_msleep?
 *       Result: k_msleep(1000) = 1000.1 ms (one extra tick), see lessons/01_getting-started/notes.md
 */
#include <zephyr/kernel.h>       /* k_msleep, k_uptime_ticks, printk */
#include <zephyr/drivers/gpio.h> /* gpio_dt_spec and the gpio_*_dt() API */

/* ---- Demo selection ---------------------------------------------------------------- */
#define DEMO_TIMED_BLINK 1
#define DEMO_SOS         2
#define DEMO_ALTERNATE   3
#define DEMO             DEMO_SOS

/* ---- Timed blink (TODO 1) ---------------------------------------------------------- */
static const int32_t sleep_time_ms = 1000;

/* ---- Morse timing (TODO 2) ----------------------------------------------------------
 * Everything is a multiple of one base unit, so changing UNIT_MS changes the speed of
 * the whole pattern. Under ~50 ms your eye can't tell dots from dashes.
 */
#define UNIT_MS        200
#define DOT_MS         (1 * UNIT_MS) /* LED on */
#define DASH_MS        (3 * UNIT_MS) /* LED on */
#define ELEMENT_GAP_MS (1 * UNIT_MS) /* LED off, between dots/dashes inside a letter */
#define LETTER_GAP_MS  (3 * UNIT_MS) /* LED off, between letters */
#define WORD_GAP_MS    (7 * UNIT_MS) /* LED off, before SOS repeats */

/*
 * One full cycle in units:
 *   S = 3 dots + 2 element gaps = 3 + 2 =  5
 *   O = 3 dashes + 2 element gaps = 9 + 2 = 11
 *   S                                    =  5
 *   2 letter gaps (S-O, O-S)    = 2 * 3  =  6
 *   word gap                             =  7
 *                                  total = 34 units
 */
#define SOS_CYCLE_UNITS 34

/*
 * Everything about the LED (controller, pin, active level) comes from the devicetree,
 * not from this file. At compile time this expands to:
 *   .port     = &gpio0 device          (DT_N_S_leds_S_d13_P_gpios_IDX_0_PH)
 *   .pin      = 13                     (..._VAL_pin)
 *   .dt_flags = GPIO_ACTIVE_HIGH (0)   (..._VAL_flags)
 * The alias "my-led" is defined in boards/esp32s3_devkitc.overlay; in C the hyphen
 * becomes an underscore because C identifiers can't contain '-'.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

/* LED 2 (TODO 3): same pattern, alias "my-led2" → node /leds/d17 → gpio0 pin 17 */
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(my_led2), gpios);

/*
 * The helpers below are static: only this file can see them, so their names can't clash
 * with other files in the firmware image and the compiler can inline them.
 *
 * They return 0 on success or the negative error code from gpio_pin_set_dt(), so a
 * failure deep inside a letter reaches main() instead of being silently ignored.
 */

/* One dot or dash: LED on for on_ms, then off for one element gap */
static int blink(int32_t on_ms)
{
	int ret;

	ret = gpio_pin_set_dt(&led, 1);
	if (ret < 0) {
		return ret;
	}
	k_msleep(on_ms);

	ret = gpio_pin_set_dt(&led, 0);
	if (ret < 0) {
		return ret;
	}
	k_msleep(ELEMENT_GAP_MS);

	return 0;
}

/*
 * Three identical elements, then the letter gap. blink() already ended with one element
 * gap of "off", so only the difference is added here, giving 3 units in total.
 */
static int send_letter(int32_t on_ms)
{
	for (int i = 0; i < 3; i++) {
		int ret = blink(on_ms);

		if (ret < 0) {
			return ret;
		}
	}
	k_msleep(LETTER_GAP_MS - ELEMENT_GAP_MS);

	return 0;
}

static int send_s(void)
{
	return send_letter(DOT_MS);  /* . . . */
}

static int send_o(void)
{
	return send_letter(DASH_MS); /* - - - */
}

/* TODO 2: repeat SOS forever. Only returns on a GPIO error. */
static int run_sos(void)
{
	int ret;
	int64_t cycle_start = k_uptime_ticks();

	printk("SOS demo: unit %d ms, ideal cycle %d units = %d us\r\n",
	       UNIT_MS, SOS_CYCLE_UNITS, SOS_CYCLE_UNITS * UNIT_MS * 1000);

	for (unsigned int n = 1; ; n++) {
		ret = send_s();
		if (ret == 0) {
			ret = send_o();
		}
		if (ret == 0) {
			ret = send_s();
		}
		if (ret < 0) {
			return ret;
		}

		/* The last S already ended with a letter gap; pad it up to a word gap */
		k_msleep(WORD_GAP_MS - LETTER_GAP_MS);

		/* One line per cycle, so the console stays readable and printk stays cheap */
		int64_t now = k_uptime_ticks();

		printk("SOS #%u  cycle: %lld us\r\n", n,
		       (long long)k_ticks_to_us_floor64(now - cycle_start));
		cycle_start = now;
	}
}

/*
 * TODO 3: the two LEDs are always opposite. One state variable drives both (LED 2 gets
 * the inverse), so they can never end up on or off together. Only returns on a GPIO error.
 */
static int run_alternate(void)
{
	int ret;
	int state = 0;

	while (1) {
		state = !state;

		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			return ret;
		}

		ret = gpio_pin_set_dt(&led2, !state);
		if (ret < 0) {
			return ret;
		}

		printk("LED1 (GPIO%d): %d   LED2 (GPIO%d): %d\r\n",
		       led.pin, state, led2.pin, !state);

		k_msleep(sleep_time_ms);
	}
}

/* TODO 1: 1 Hz blink with timing measurements. Only returns on a GPIO error. */
static int run_timed_blink(void)
{
	int ret;
	int state = 0;

	/*
	 * Timing uses kernel ticks (CONFIG_SYS_CLOCK_TICKS_PER_SEC=10000, so 1 tick = 0.1 ms)
	 * instead of k_uptime_get(), whose whole milliseconds hide the fractions.
	 */
	int64_t last_toggle = k_uptime_ticks();

	while (1) {
		state = !state;

		/* Logical level: 1 = "active", whatever that means electrically for this LED */
		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			return ret;
		}

		/*
		 * Full loop period, toggle to toggle. Includes printk and GPIO time, which turn
		 * out to be < 1 tick: printk just fills the UART's hardware FIFO and returns.
		 * (The very first period is 0, and the second is longer because the boot banner
		 * filled the FIFO.)
		 */
		int64_t now = k_uptime_ticks();
		int64_t period_us = k_ticks_to_us_floor64(now - last_toggle);

		last_toggle = now;

		printk("LED state: %d  period: %lld us\r\n", state, (long long)period_us);

		/*
		 * The sleep alone. Measures 1000.1 ms: sleeps last *at least* the requested time,
		 * so the kernel adds one tick because we start partway through a tick. That
		 * +0.1 ms per loop accumulates (drift); an absolute deadline or a k_timer avoids it.
		 * While we sleep, the scheduler runs the idle thread, which waits in low power.
		 */
		int64_t sleep_start = k_uptime_ticks();

		k_msleep(sleep_time_ms);
		int64_t sleep_us = k_ticks_to_us_floor64(k_uptime_ticks() - sleep_start);

		/* (long long) cast: int64_t may be long or long long; %lld needs long long */
		printk("  k_msleep(%d) took %lld us\r\n", sleep_time_ms, (long long)sleep_us);
	}
}

int main(void)
{
	int ret;

	/*
	 * Checks the GPIO *controller* (led.port), not the LED: its driver init ran at
	 * boot, before main(). A missing pin would have failed the build instead.
	 */
	if (!gpio_is_ready_dt(&led)) {
		printk("Error: LED GPIO not ready\r\n");
		return 0;
	}

	/*
	 * Both LEDs happen to be on gpio0, so this checks the same controller again. Keep it:
	 * if LED 2 ever moves to gpio1 (GPIO 32+) or an I/O expander, the check still holds.
	 */
	if (!gpio_is_ready_dt(&led2)) {
		printk("Error: LED2 GPIO not ready\r\n");
		return 0;
	}

	/*
	 * Output mode, starting inactive (off); the DT flags (active high/low) are applied
	 * automatically. Configuring LED 2 in every demo keeps it in a known, off state.
	 */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: could not configure LED\r\n", ret);
		return 0;
	}

	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: could not configure LED2\r\n", ret);
		return 0;
	}

	/*
	 * A plain if (not #if) so both demos are always compiled and type-checked; the
	 * compiler drops the unused one because DEMO is a compile-time constant.
	 */
	if (DEMO == DEMO_SOS) {
		ret = run_sos();
	} else if (DEMO == DEMO_ALTERNATE) {
		ret = run_alternate();
	} else {
		ret = run_timed_blink();
	}

	/* The demos loop forever, so getting here means a GPIO call failed */
	printk("Error %d: could not set LED\r\n", ret);

	return 0;
}
