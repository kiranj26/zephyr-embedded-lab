/*
 * Lesson 9 — Interrupts and workqueues
 *   1. GPIO interrupt: configure GPIO_INT_EDGE_TO_ACTIVE, gpio_init_callback, gpio_add_callback.
 *      In the ISR, count presses. Press once — how many interrupts do you get? (bounce!)
 *   2. k_timer demo: a periodic timer that prints (ISR context — what can't you do here?)
 *   3. Challenge: debounce with k_work_delayable:
 *        ISR -> k_work_reschedule(&work, K_MSEC(50));
 *        handler (thread context) -> read pin again, act only if still pressed
 * Measure: raw interrupt count vs debounced presses; print both.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define DEBOUNCE_MS 50

static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);
static struct gpio_callback btn_cb;
static atomic_t raw_irqs = ATOMIC_INIT(0);
/* TODO: static struct k_work_delayable btn_work; */

static void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	atomic_inc(&raw_irqs);
	/* TODO: k_work_reschedule(...) */
}

/* TODO: static void button_work_handler(struct k_work *work) { ... } */

int main(void)
{
	if (!gpio_is_ready_dt(&btn)) {
		printk("button not ready\r\n");
		return 0;
	}
	gpio_pin_configure_dt(&btn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&btn_cb, button_isr, BIT(btn.pin));
	gpio_add_callback(btn.port, &btn_cb);

	/* TODO: k_work_init_delayable(&btn_work, button_work_handler); */

	while (1) {
		k_msleep(2000);
		printk("raw interrupts so far: %ld\r\n", (long)atomic_get(&raw_irqs));
	}
	return 0;
}
