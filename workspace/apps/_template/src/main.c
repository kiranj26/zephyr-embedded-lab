#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
	printk("App template running\r\n");

	while (1) {
		/* TODO: your code */
		k_msleep(1000);
	}

	return 0;
}
