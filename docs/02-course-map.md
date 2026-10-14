# 02 · Course map — playlist ↔ blog ↔ repo ↔ challenge

**Series:** Introduction to Zephyr, Shawn Hymel for DigiKey (published weekly from March 2025)
**Playlist:** https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52
**Repo (submodule `course/`):** https://github.com/ShawnHymel/introduction-to-zephyr
**Written versions:** DigiKey Maker.io "Introduction to Zephyr Part N" posts (links below). If a link 404s, search the title on digikey.com/en/maker.

Pattern for every episode: **demo** (shown in the video) → **challenge** (given at the end) → **solution** (in the repo, and usually explained in the blog post).

| Part | Title | Demo folder(s) | Challenge | Solution folder | Blog |
|---|---|---|---|---|---|
| 1 | Getting Started – Installation and Blink | `01_blink` | — (setup) | — | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-1-getting-started-installation-and-blink) |
| 2 | CMake Tutorial | `02_demo_cmake` | add a hello library to blink | `02_solution_hello_blink` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-2-cmake-tutorial) |
| 3 | Kconfig Tutorial | `03_demo_kconfig`, `modules/say_hello` | print random as float | `03_solution_kconfig_float` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-3-kconfig-tutorial) |
| 4 | Devicetree Tutorial | `04_demo_button` | button controls LED | `04_solution_button_led` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-4-devicetree-tutorial) |
| 5 | Devicetree Bindings | `05_demo_adc` | knob → PWM LED brightness | `05_solution_pwm_knob` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-5-devicetree-bindings) |
| 6 | Device Driver Development | `06_demo_device_driver`, `modules/button` | [workshop-zephyr-device-driver](https://github.com/ShawnHymel/workshop-zephyr-device-driver) (MCP9808) | `06_solution_i2c_driver`, `modules/mcp9808` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-6-device-driver-development) |
| 7 | Debugging with OpenOCD and GDB | `01_blink` (as target) | console over USB port | `07_solution_usb_print` ¹ | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-7-debugging-with-openocd-and-gdb) |
| 8 | Multithreading | `08_demo_multithreading`, `08_demo_mutex` | sensor thread → queue → printer thread | `08_solution_sensor_queue` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-8-multithreading) |
| 9 | Interrupts and Workqueues | `09_demo_gpio_interrupt`, `09_demo_timer`, `09_demo_counter_interrupt` | debounce with a workqueue | `09_solution_workqueue` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-9-interrupts-and-workqueues) |
| 10 | Graphics with LVGL and Display Drivers | `10_demo_display` | animate a shape | `10_solution_animation` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-10-graphics-with-lvgl-and-display-drivers) |
| 11 | WiFi and IoT | `11_demo_wifi` | use the HTTP client subsystem | `11_solution_http` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-11-wifi-and-iot) |
| 12 | How to Create a Custom Board Definition | `boards/custom/nyan`, `12_demo_custom_board` | board with the display built in | `boards/custom/my_board`, `12_solution_custom_board` | [post](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-12-how-to-create-a-custom-board-definition) |

¹ The Part 7 blog's "solution" link points to `08_solution_sensor_queue`. That's a copy-paste slip; the USB-console solution is `07_solution_usb_print`.

## Course repo structure (what each top-level piece is for)
```
course/
├── Dockerfile.espressif      ← the image we build (Zephyr pinned commit, SDK 0.16.8, code-server)
├── Dockerfile.rp2            ← Raspberry Pi Pico variant (not used)
├── scripts/espressif/west.yml← trimmed west manifest (only modules ESP32 needs)
├── scripts/entrypoint.sh     ← starts sshd + code-server
├── scripts/zephyr.code-workspace
├── .devcontainer/            ← VS Code Dev Containers config (alternative to our `make up`)
└── workspace/                ← mounted at /course (read-only) in this lab
    ├── apps/NN_demo_* / NN_solution_*
    ├── modules/{say_hello, button, mcp9808}   ← out-of-tree modules (lessons 3, 6)
    ├── boards/custom/{nyan, my_board}         ← lesson 12
    └── .vscode/launch.json                    ← GDB config (lesson 7)
```

## This lab's mapping
Starter apps live in `workspace/apps/NN_*`, guides in `lessons/NN_*`. See `lessons/README.md`.
