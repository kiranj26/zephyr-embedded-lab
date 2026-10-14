# Notes — 01_getting-started

Started: 2026-10-14 · Finished:

## The idea in my own words (3–5 sentences)
_TODO: write this myself after watching the video (see the prompt at the bottom)._

## Key concepts
- **Build/flash split:** Docker builds the firmware, but Docker on macOS can't reach USB, so the Mac flashes and reads the console (esptool + pyserial in `venv/`).
- **Two USB-C ports:** the "UART" port goes through a USB-to-UART chip to UART0 on GPIO43/44. That's where flashing and `printk` happen. The "USB" port is the ESP32's own USB (used for JTAG and the USB console in lesson 7).
- **Board target** `esp32s3_devkitc/esp32s3/procpu` = board / SoC / CPU cluster.
- **Node identifier:** my words: *"DT_ALIAS(my_led) is a node id and gpios is the prop"* ✅. The alias is just a macro that expands to a path-encoded node name: `DT_N_S_leds_S_d13` = `/leds/d13`.
- **Hyphen becomes underscore:** C names can't contain `-`, so the generator lowercases names and replaces every non-alphanumeric character with `_` (`my-led` → `my_led`).
- **`gpio_dt_spec`** = `.port` + `.pin` + `.dt_flags`. My first guess was right about the three fields. Correction: `.port` is a **pointer to the GPIO controller's device object**, not a number.
- **Readiness:** `gpio_is_ready_dt()` checks the **controller** (`led.port`), not the LED. Device init functions run at boot, before `main()`. "Not ready" means that init failed. A missing pin or port can't be the reason, because that fails at **build** time. This matters most for external chips, like an I2C GPIO expander that isn't plugged in.
- **Logical vs physical:** `_dt` setters take a *logical* state, and the DT flags say what "active" means. Switching to `GPIO_ACTIVE_LOW` needs no C change, but inverts the pin level. The `_raw` setters drive the physical level.
- **Sleeping:** `k_msleep()` puts the thread to sleep. The scheduler then runs the idle thread, which waits in low power (`waiti`) until the timer interrupt. There's no busy loop.
- **DT layers:** the SoC `.dtsi` describes the chip, the board `.dts` adds what's on the board, and my `.overlay` adds my parts. `&gpio0` *refers to* a node defined lower down. It doesn't create one.
- **DT ↔ Kconfig:** devicetree = what hardware exists. Kconfig = what software gets built. They're linked by `CONFIG_DT_HAS_<compatible>_ENABLED`.
- **Two switches (opt-in on both sides):** a driver is built only if **(1)** its subsystem is on in Kconfig (`CONFIG_I2C`, `CONFIG_WIFI`, …) **and (2)** a node with a matching `compatible` has `status = "okay"`. An empty `prj.conf` does *not* build every peripheral. In blink: `CONFIG_SERIAL=y` → `UART_ESP32=y`, but I2C, SPI, ADC, PWM and Wi-Fi are all unset. 31 DT nodes are "disabled" and 11 are "okay". Wi-Fi is off twice: `# CONFIG_WIFI is not set` and `wifi { status = "disabled"; }` (zephyr.dts:518). Lesson 11 needs **both**: `CONFIG_WIFI=y` + `&wifi { status = "okay"; };`.

## APIs / macros
| Name | What it does | Header / file |
|---|---|---|
| `DT_ALIAS(name)` | Node identifier for an entry in `/aliases` | `zephyr/devicetree.h` |
| `GPIO_DT_SPEC_GET(node, prop)` | Builds a `gpio_dt_spec` (port, pin, flags) at compile time | `zephyr/drivers/gpio.h` |
| `DEVICE_DT_GET(node)` | Compile-time pointer to a node's `struct device` | `zephyr/device.h` |
| `gpio_is_ready_dt(&spec)` | `device_is_ready(spec.port)`: did the controller initialize OK? | `zephyr/drivers/gpio.h` |
| `gpio_pin_configure_dt(&spec, GPIO_OUTPUT)` | Configure the pin with its DT flags plus extra flags | `zephyr/drivers/gpio.h` |
| `gpio_pin_set_dt(&spec, v)` | Set the **logical** level (respects ACTIVE_LOW) | `zephyr/drivers/gpio.h` |
| `k_msleep(ms)` | Sleep the current thread; the CPU runs other threads or idles | `zephyr/kernel.h` |
| `k_uptime_get()` | Uptime in whole ms (`int64_t`) | `zephyr/kernel.h` |
| `k_uptime_ticks()` | Uptime in kernel ticks (here 1 tick = 0.1 ms) | `zephyr/kernel.h` |
| `k_ticks_to_us_floor64(t)` | Convert ticks to µs, rounding down | `zephyr/sys/time_units.h` |
| `printk()` | Kernel print to the console (UART0) | `zephyr/sys/printk.h` |

## Kconfig symbols · Devicetree properties
- `CONFIG_GPIO=y`: set by the **board defconfig** (`boards/espressif/esp32s3_devkitc/esp32s3_devkitc_procpu_defconfig:8`).
- `CONFIG_GPIO_ESP32=y`: `default y` + `depends on DT_HAS_ESPRESSIF_ESP32_GPIO_ENABLED`. It's turned on **by the devicetree**.
- `CONFIG_DT_HAS_ESPRESSIF_ESP32_GPIO_ENABLED=y`: generated into `build/.../Kconfig/Kconfig.dts` because a node with `compatible = "espressif,esp32-gpio"` has `status = "okay"`.
- My overlay:
  ```dts
  / {
      aliases { my-led = &led0; };
      leds {
          compatible = "gpio-leds";
          led0: d13 { gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>; };
      };
  };
  ```
- **GPIO controllers:** `gpio0` (`0x60004000`, `ngpios = 32`) covers GPIO 0–31. `gpio1` (`0x60004800`, `ngpios = 22`) covers GPIO 32+, with **cell = GPIO − 32**. For example, GPIO38 is written `<&gpio1 6 ...>`. Pinctrl is for peripherals (UART/SPI/I2C), not plain GPIO.

## What I found in build output (zephyr.dts / .config / generated headers / map)
- `devicetree_generated.h`:
  ```c
  #define DT_N_ALIAS_my_led                    DT_N_S_leds_S_d13
  #define DT_N_S_leds_S_d13_P_gpios_IDX_0_PH   DT_N_S_soc_S_gpio_S_gpio_60004000  // → .port
  #define DT_N_S_leds_S_d13_P_gpios_IDX_0_VAL_pin    13                         // → .pin
  #define DT_N_S_leds_S_d13_P_gpios_IDX_0_VAL_flags  0                          // → .dt_flags
  ```
  `_S_` = `/`, `_P_` = property, `PH` = phandle (the node it points to).
- The pin and flags come from **my overlay**. The controller node `gpio0: gpio@60004000` comes from `dts/xtensa/espressif/esp32s3/esp32s3_common.dtsi` (merged result: `zephyr.dts:188`).
- **k_msleep accuracy (TODO 1):** `k_msleep(1000)` took **1000.1 ms** (10 001 ticks) every time: exactly one extra tick, because sleeps are "at least" the requested time and the call starts partway through a tick. With `k_uptime_get()` (whole ms) this showed up as 1000, or now and then 1001: the extra 0.1 ms only crosses a ms boundary about 1 time in 10. Never 999. The toggle-to-toggle period was also 1000.1 ms, so printk + GPIO cost too little to show at 0.1 ms resolution. printk most likely just drops bytes into the UART's hardware FIFO and returns. The first period was 1002.5 ms, probably because the boot banner filled the FIFO so printk had to wait. Drift = +0.1 ms per loop, about 0.36 s/hour. An absolute deadline or a periodic `k_timer` (lesson 9) avoids it.
- **SOS cycle (TODO 2):** 34 units × 200 ms = 6,800,000 µs ideal. Measured **6,802,200 µs**, identical every cycle. The extra 2,200 µs = 22 ticks = **22 `k_msleep` calls per cycle** (18 in `blink()` + 3 letter pads + 1 word pad). Same +1-tick-per-sleep rule as TODO 1, so drift grows with the *number of sleeps*. Full write-up in `todo2-sos-guide.md` §9.
- **Second LED (TODO 3):** overlay adds `led1: d17 { gpios = <&gpio0 17 GPIO_ACTIVE_HIGH>; }` + alias `my-led2`. Generated: `DT_N_ALIAS_my_led2 → DT_N_S_leds_S_d17`, `_VAL_pin 17`, `_PH → gpio@60004000` (same controller as LED 1). In `zephyr.dts` the pin shows as `0x11` (hex for 17). Serial shows `LED1 (GPIO13): 1   LED2 (GPIO17): 0` alternating every second. GPIO17 chosen with the 7-check method in `gpio-analysis.md`.
- The build's memory table says FLASH is about 8 MB, but my module has 16 MB. The build trusts the board definition, not the chip (→ lesson 12).

## Bugs I hit → root cause → fix
- **`make image` failed at the very end** ("lease does not exist"). Docker Desktop's containerd timed out during a roughly 1 h 44 min layer export. → Rerun `make image`. The cache made it finish in about 24 s.
- **Board on the wrong port:** `cu.usbmodem31401` was the native **USB** port, so the console would have been silent. → Moved the cable to **UART**.
- **The UART port isn't named `usbserial`** on this clone board. It shows as `cu.usbmodem5CBC4009281` (probably a CH343 chip). It works, but don't plug both ports in at once.
- **`make menuconfig` gave "No rule to make target"**: I ran it inside the container, but the Makefile lives on the Mac. → Run it in a Mac terminal at the repo root, or inside the container use `west build -d /workspace/build/01_blink -t menuconfig`. It also needs an existing build first.

## Challenge: my approach vs the course solution
No formal challenge in this lesson. Extensions: [x] print uptime (k_msleep = +1 tick) · [x] SOS · [x] second LED on **GPIO17** (`DEMO_ALTERNATE`).
- **SOS design:** helper functions with a shared `send_letter(on_ms)`; every timing derived from `UNIT_MS`; helpers are `static` and return GPIO errors up to `main()`; one printk per cycle; old blink kept behind `#define DEMO`. Each gap is built as "element gap + pad" so the arithmetic never double-counts.
- **Lesson:** `#define` = untyped text substitution; `static const` = typed object (visible in GDB, but not a compile-time constant in C); `enum` = typed integer compile-time constant.

## Open questions
- [ ] Try `GPIO_ACTIVE_LOW` myself and confirm `_VAL_flags` becomes `1` and the LED looks inverted.
- [ ] In menuconfig, use `/` then `?` on `GPIO` and `GPIO_ESP32` to see "defined at" and "selected by" for myself.
- [ ] What actually happens before `main()`? Init levels (PRE_KERNEL / POST_KERNEL / APPLICATION).

## Next time / ideas
- **Demo video for the GitHub README:** record once all three TODOs are done. Show the LED doing SOS (and the two-LED version) next to `make monitor` output.
- Watch Part 1 (not yet ticked in PROGRESS).
- Write "the idea in my own words" above, then do the extensions, then `/review 01`.
