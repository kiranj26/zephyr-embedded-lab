---
name: course-map
description: Maps the 12 Introduction to Zephyr video episodes to their DigiKey blog posts, course repo folders, challenges, solutions, hardware, and this lab's lesson guides and starter apps. Use whenever the user references an episode/part number, a course folder, or asks "what's next".
---
# Course map (Shawn Hymel × DigiKey, 2025)

Playlist: https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52 ·
Repo: https://github.com/ShawnHymel/introduction-to-zephyr (submodule `course/`)

| Part | Title | Course demo → challenge solution | Lab starter |
|---|---|---|---|
| 1 | Getting Started – Installation and Blink | `01_blink` | `01_blink` |
| 2 | CMake Tutorial | `02_demo_cmake` → `02_solution_hello_blink` | `02_hello_lib` |
| 3 | Kconfig Tutorial | `03_demo_kconfig` (+`modules/say_hello`) → `03_solution_kconfig_float` | `03_kconfig_random` |
| 4 | Devicetree Tutorial | `04_demo_button` → `04_solution_button_led` | `04_button_led` |
| 5 | Devicetree Bindings | `05_demo_adc` → `05_solution_pwm_knob` | `05_pwm_knob` |
| 6 | Device Driver Development | `06_demo_device_driver` (+`modules/button`) → workshop → `06_solution_i2c_driver` (+`modules/mcp9808`) | `06_my_driver` |
| 7 | Debugging with OpenOCD and GDB | uses `01_blink` → `07_solution_usb_print` | `07_usb_console` |
| 8 | Multithreading | `08_demo_multithreading`, `08_demo_mutex` → `08_solution_sensor_queue` | `08_sensor_queue` |
| 9 | Interrupts and Workqueues | `09_demo_gpio_interrupt`, `09_demo_timer`, `09_demo_counter_interrupt` → `09_solution_workqueue` | `09_debounce` |
| 10 | Graphics with LVGL and Display Drivers | `10_demo_display` → `10_solution_animation` | `10_lvgl_ui` |
| 11 | WiFi and IoT | `11_demo_wifi` → `11_solution_http` | `11_http_client` |
| 12 | How to Create a Custom Board Definition | `boards/custom/nyan` + `12_demo_custom_board` → `boards/custom/my_board` + `12_solution_custom_board` | `12_board_blink` + `boards/custom/s3_n16r8` |

Notes:
- The Part 7 blog's "solution" link points to `08_solution_sensor_queue`, but the Part 7 challenge (printk over the USB port) is `07_solution_usb_print`.
- The Part 6 challenge is a separate repo: github.com/ShawnHymel/workshop-zephyr-device-driver (MCP9808 I2C driver).
- Lesson guides: `lessons/NN_*/README.md`. Full table with blog links: `docs/02-course-map.md`.
