# Progress — Zephyr Embedded Lab

Legend: ☐ todo · ◐ in progress · ☑ done. Ask Claude Code to `/progress` at the end of each session.

| # | Episode | Watched | Ref runs | My build | Challenge | Deeper ×2 | Notes | Date |
|---|---|---|---|---|---|---|---|---|
| 00 | Setup (Mac, Docker, first flash) | – | ☑ | ☑ | – | – | ☑ | 2026-10-14 |
| 01 | Getting Started – Blink | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-14 |
| 02 | CMake | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-15 |
| 03 | Kconfig | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-16 |
| 04 | Devicetree | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | 2026-10-17 |
| 05 | Devicetree Bindings | ☑ | ☑ | ☑ | ◐ | ☐ | ◐ | |
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
- Kconfig off = NOT defined (not 0); prefer IS_ENABLED(CONFIG_X)
- Config layers: defconfig < prj.conf < boards/<board>.conf < -DCONFIG_X; menuconfig only edits build/.config (lost on pristine build)
- depends on = requirement, select = forces; Zephyr aborts on Kconfig warnings (range, unmet deps, typos)
- ENTROPY_GENERATOR=y → ESP32 TRNG driver (DT trng0) → hardware random; TEST_RANDOM_GENERATOR = fake, tests only
- FPU = float math; CBPRINTF_FP_SUPPORT = float printing (else "*float*"); with picolibc, printk uses picolibc vfprintf (+828 B for float)
- Kconfig: range/unknown → build aborts; unmet depends on → only a warning (value silently dropped)
- sys_rand*() falls back silently to k_cycle_get_32() on driver failure; sys_csrand_get() returns the error. ESP32 RNG = real entropy only with Wi-Fi/BT on
- Button: GPIO_ACTIVE_LOW | GPIO_PULL_UP (0x11) → gpio_pin_get_dt() = 1 when pressed; configure_dt merges dt_flags, so C only says GPIO_INPUT
- DT labels must be unique across the merged tree (board already has button0); alias = name for C, label = name inside DT
- Floating input: CMOS input ≈ pF + GΩ → wire = antenna → random reads; pull-up pins it. PULL_UP = stability, ACTIVE_LOW = interpretation; wrong flags still build
- DT: same path = merged into existing node; &{/path} extends deliberately; DT boolean = true only if present (gpio-keys polling-mode default off)
- Bindings: properties/child-binding/required/enum/const; follow include:. Missing required → build error; enum value chip can't do → runtime -ENOTSUP
- ADC: zephyr,user io-channels (consumer) + &adc0 channel@N (provider); ESP32 "raw" is calibrated, mV = raw*4400/4096; vref-mv ignored with ADC_REF_INTERNAL
- PWM API in ns; LEDC 8 ch/4 timers; period vs resolution (80 MHz/f, max 14 bit) → 1 kHz for LEDs; pinctrl routes peripheral signal → pin, applied via pinctrl-0/names
- Overlay macros need their header (#include <zephyr/dt-bindings/pwm/pwm.h>); 64-bit multiply avoids period×value overflow
- zephyr_interface = INTERFACE target carrying Zephyr includes, -imacros autoconf.h, defines, CPU flags (type, debugger visibility, compile-time constant)
