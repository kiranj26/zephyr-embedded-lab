/*
 * Lesson 10 — LVGL (v8 API!)
 *   1. Get the display: DEVICE_DT_GET(DT_CHOSEN(zephyr_display)); device_is_ready()
 *   2. "Hello, World!" label at LV_ALIGN_TOP_MID; counter label at bottom (font montserrat_20)
 *   3. Draw a rectangle with lv_line (5 points) and a red circle (lv_obj + radius style)
 *   4. display_blanking_off(); loop: lv_task_handler() every 50 ms
 *   Challenge: animate the circle left->right across lv_disp_get_hor_res(NULL)
 *   Go deeper: show live MCP9808 temperature (reuse lesson 8's queue).
 * Rule: lv_style_t objects must outlive the objects using them (make them static).
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

int main(void)
{
	const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

	if (!device_is_ready(display)) {
		printk("Error: display not ready\r\n");
		return 0;
	}

	/* TODO: widgets */

	display_blanking_off(display);

	while (1) {
		lv_task_handler();
		k_msleep(50);
	}
	return 0;
}
