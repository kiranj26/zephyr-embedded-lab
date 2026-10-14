/*
 * Lesson 8 — Multithreading
 * Demo practice (do these first in this file, then delete):
 *   a) blink thread (priority 7) + main printing "Hello" every 700 ms
 *   b) mutex: shared blink period changed from console input (+/-)  [CONFIG_CONSOLE_SUBSYS, CONFIG_CONSOLE_GETLINE]
 * Challenge — producer/consumer with a message queue:
 *   - sensor thread: every 500 ms fetch temperature -> k_msgq_put(&q, &val, K_NO_WAIT)
 *   - output thread: every 5 s drain the queue and print all readings
 * Questions to answer in notes: what happens when the queue is full? Which thread should have higher priority, and why?
 */
#include <zephyr/kernel.h>
/* #include <zephyr/drivers/sensor.h> */

#define SENSOR_STACK_SIZE 512
#define OUTPUT_STACK_SIZE 1024
#define QUEUE_LEN 20

/* TODO: K_THREAD_STACK_DEFINE x2, struct k_thread x2, K_MSGQ_DEFINE(...) */

/* TODO: void sensor_thread_start(void *dev, void *a2, void *a3) { ... } */
/* TODO: void output_thread_start(void *a1, void *a2, void *a3) { ... } */

int main(void)
{
	printk("Lesson 8 starter\r\n");
	/* TODO: k_thread_create(...) x2 */
	while (1) {
		k_msleep(1000);
	}
	return 0;
}
