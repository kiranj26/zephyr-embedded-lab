---
name: lvgl-display
description: LVGL v8 + ST7735R (160x80, MIPI-DBI over SPI) on the pinned Zephyr version - required Kconfig, overlay, widget/style API, animation loop, and crash causes. Use for lesson 10, lesson 12 solution, and any GUI work.
---
# LVGL (v8 API) on ST7735R 160×80

- **Kconfig:** copy `course/workspace/apps/10_solution_animation/prj.conf` (`MAIN_STACK_SIZE=4096`, `DISPLAY`, `LVGL`,
  `LV_MEM_CUSTOM`, `LV_Z_BITS_PER_PIXEL=16`, `LV_COLOR_16_SWAP`, `LV_Z_MEM_POOL_SIZE=8192`, `LV_CONF_MINIMAL` + widgets, fonts).
- **Overlay:** see the devicetree-overlays skill. The `x-offset=1`/`y-offset=26` values and the gamma arrays are panel-specific.
  `madctl` sets rotation and RGB order.
- **API (v8, do not use v9 names):** `lv_scr_act()`, `lv_label_create`, `lv_label_set_text`, `lv_obj_align(obj, LV_ALIGN_*, x, y)`,
  `lv_style_init/lv_style_set_*`, `lv_obj_add_style`, `lv_line_create` + `lv_line_set_points`, `lv_disp_get_hor_res(NULL)`,
  `lv_color_hex`, `lv_font_montserrat_20` (needs `CONFIG_LV_FONT_MONTSERRAT_20=y`).
- **Loop:** call `lv_task_handler()` periodically (every 5–50 ms). Call `display_blanking_off(display)` once after setup.
- **Styles must outlive their objects.** Make `lv_style_t` `static` or global. Stack-allocated styles in a function that returns are a bug.
  (The course code gets away with it because main never returns.)
- **Crashes:** stack overflow means raising `MAIN_STACK_SIZE`. Allocation failures mean raising `LV_Z_MEM_POOL_SIZE`.
  A white/garbled screen means checking wiring, `colmod`, offsets, and `rgb-is-inverted`.
