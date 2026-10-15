# Progress — Zephyr Embedded Lab

Legend: ☐ todo · ◐ in progress · ☑ done. Ask Claude Code to `/progress` at the end of each session.

| # | Episode | Watched | Ref runs | My build | Challenge | Deeper ×2 | Notes | Date |
|---|---|---|---|---|---|---|---|---|
| 00 | Setup (Mac, Docker, first flash) | – | ☑ | ☑ | – | – | ☑ | 2026-10-14 |
| 01 | Getting Started – Blink | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-14 |
| 02 | CMake | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-15 |
| 03 | Kconfig | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 04 | Devicetree | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 05 | Devicetree Bindings | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 06 | Device Drivers | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 07 | Debugging (OpenOCD/GDB) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 08 | Multithreading | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 09 | Interrupts & Workqueues | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 10 | LVGL & Display | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 11 | WiFi & IoT | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| 12 | Custom Board | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |
| ★ | Capstone: desk weather node | | | | | | | |

## Hardware
- [x] Breadboard(s) + jumpers  - [x] LEDs + 330 Ω  - [ ] 2 push buttons  - [ ] 10 kΩ pot
- [ ] MCP9808 breakout (Adafruit 1782 or similar)  - [ ] ST7735R 0.96" 160×80 (Adafruit 3533 or similar)
- [ ] 2 × USB-C **data** cables  - [ ] (optional) logic analyzer, multimeter

## Spaced-repetition queue (concepts to re-explain from memory)
- Why Docker builds but the Mac flashes (no USB passthrough on macOS)
- UART port vs native USB port; console lives on UART0 (GPIO43/44)
- DT_ALIAS(my_led) → DT_N_ALIAS_my_led → DT_N_S_leds_S_d13 (_S_ = '/', _P_ = property, hyphen → underscore)
- gpio_dt_spec = .port (controller device ptr) + .pin + .dt_flags; gpio_is_ready_dt checks the controller's boot-time init
- GPIO 32–48 live on gpio1 (cell = GPIO − 32), not gpio0
- DT compatible + status=okay → CONFIG_DT_HAS_*_ENABLED → driver Kconfig `depends on` → driver auto-enabled
- GPIO_ACTIVE_LOW: same C code, inverted physical level (_dt = logical, _raw = physical)
- k_msleep → idle thread → waiti low-power, no busy loop
- Every k_msleep adds +1 tick → drift scales with the number of sleeps (SOS: 22 sleeps = +2.2 ms per cycle)
- A pinctrl group only applies when its device is enabled (GPIO17 in i2s0_default is harmless while i2s0 is disabled)
- GPIO_OUTPUT_INACTIVE vs GPIO_OUTPUT: start in a known (off) state
- #define vs static const vs enum
- Header path missing = compile error (fatal error: x.h); missing code = link error (undefined reference)
- PRIVATE / PUBLIC / INTERFACE propagation; `app` is a library Zephyr creates in find_package(Zephyr)
- printf cost depends on config: picolibc vfprintf is always linked here, so printf ≈ +60 B; GCC turns constant printf into puts
- zephyr_library helpers auto-link only in modules; in an app still need target_link_libraries(app …)
- zephyr_interface = INTERFACE target carrying Zephyr includes, -imacros autoconf.h, defines, CPU flags (type, debugger visibility, compile-time constant)
