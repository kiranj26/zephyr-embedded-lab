# Notes — 04_devicetree

Started: 2026-10-16 · Finished: 2026-10-17

## The idea in my own words (3–5 sentences)
The devicetree is a text description of the hardware, and in Zephyr it's processed at **build time** into C macros, so it costs no flash and nothing is parsed on the chip. My overlay adds a `gpio-keys` node for the button and a `gpio-leds` node for the LED, plus aliases that give C a portable name. `GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios)` turns that into a struct holding the controller, the pin and the flags. The flags carry the electrical meaning: `GPIO_PULL_UP` keeps the input from floating, and `GPIO_ACTIVE_LOW` makes "pressed" (0 V) read as logical 1, so my C code only deals with pressed/released and on/off. If a flag is wrong the build still passes and only the hardware misbehaves, so I check the merged `zephyr.dts` and print `dt_flags` at startup.

## Key concepts
- **Linux vs Zephyr:** Linux loads the devicetree at boot and reads it at runtime. Zephyr turns it into **compile-time macros** (`devicetree_generated.h`).
- **Vocabulary:**

  | Term | Meaning | Example |
  |---|---|---|
  | node | one piece of hardware (or a group) | `/gpio-keys/user-button` |
  | property | a fact about it | `gpios`, `status`, `reg`, `compatible` |
  | `compatible` | the node's type: picks the **binding** (and a driver, if one is built) | `"gpio-keys"` |
  | label | a nickname **inside devicetree files** (`&label`) | `button1:` |
  | phandle | a reference to another node | `&gpio0` |
  | alias | a name **for C** (`DT_ALIAS`) | `my-button` |

- **Label vs alias:** a label lets devicetree files point at each other. An alias lets C find a node without knowing its path.
- **Layers:** SoC `.dtsi` → board `.dts` → my overlay → **`build/…/zephyr/zephyr.dts`** (the final merged tree). An overlay can add nodes, modify existing ones (`&label { … };` or `&{/path} { … };`), or reference them.
- **Same path = merged.** A node in the overlay with the same path as an existing one is **merged into it**: its properties are added or overridden. A different path is a new node.
- **Labels must be unique** across the whole merged tree. The board already has `button0:` (its BOOT button on GPIO0), so reusing it is a `dtc` duplicate-label error.
- **Bindings** (YAML) define which properties a `compatible` allows and requires, with their types. They validate the overlay, and decide which macros are generated (no binding means no `_P_` macros).
- **Input pins:** button wired GPIO5 → switch → GND:
  - `GPIO_PULL_UP` (`1<<4`): released = 3.3 V instead of floating
  - `GPIO_ACTIVE_LOW` (`1<<0`): pressed (0 V) = logical 1
  - together `0x11`, the same as the board's BOOT button
- **`gpio_pin_configure_dt()` merges `.dt_flags`**, so C only says `GPIO_INPUT`, and all electrical detail stays in the overlay.
- **Floating input physics:** a CMOS input is a tiny capacitance with MΩ–GΩ resistance, so a few picocoulombs move its voltage. An unconnected wire acts as an antenna (mains 50/60 Hz, my hand's capacitive coupling even without touching, nearby signals) and drifts across the switching threshold (roughly 1/3 to 2/3 of VDD). The pull-up (about 45 kΩ internal) drains that charge and pins the level. Pressed current is only 3.3 V / 45 kΩ ≈ 73 µA.
- **Two switches again:** `gpio-keys` and `gpio-leds` *do* have drivers (`INPUT_GPIO_KEYS`, `LED_GPIO`: `default y`, `depends on DT_HAS_…_ENABLED`), but they live in the `INPUT` and `LED` menus, which are off. So the nodes are **pure data**, and my code drives the GPIO controller directly.

## APIs / macros
| Name | What it does | Header |
|---|---|---|
| `DT_ALIAS(name)` | node ID for an alias (`-` → `_`) | `zephyr/devicetree.h` |
| `GPIO_DT_SPEC_GET(node, gpios)` | `{ .port, .pin, .dt_flags }` from the devicetree | `zephyr/drivers/gpio.h` |
| `gpio_pin_configure_dt(&spec, GPIO_INPUT)` | input + `dt_flags` (pull-up, active-low) | `zephyr/drivers/gpio.h` |
| `gpio_pin_get_dt(&spec)` | **logical** level (1 = active/pressed), or negative error | `zephyr/drivers/gpio.h` |
| `GPIO_OUTPUT_INACTIVE` | output, starting in logical off | `zephyr/drivers/gpio.h` |
| `GPIO_ACTIVE_LOW` / `GPIO_PULL_UP` | `1<<0` / `1<<4` | `zephyr/dt-bindings/gpio/gpio.h` |
| `DT_FOREACH_CHILD(node, fn)` | run `fn` over every child of a node | `zephyr/devicetree.h` |

## Kconfig symbols · Devicetree properties
- `CONFIG_GPIO=y` (stated in `prj.conf`; also from the board defconfig)
- `CONFIG_DT_HAS_GPIO_KEYS_ENABLED=y`, `CONFIG_DT_HAS_GPIO_LEDS_ENABLED=y` (nodes exist), but `# CONFIG_INPUT is not set`, `# CONFIG_LED is not set` (no drivers)
- **My overlay:**
  ```dts
  aliases { my-button = &button1; my-led = &led1; };
  gpio-keys { compatible = "gpio-keys";
      button1: user-button { gpios = <&gpio0 5 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>; label = "User Button"; }; };
  gpio-leds { compatible = "gpio-leds";
      led1: user-led { gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>; label = "User LED"; }; };
  ```
- **`gpio-keys` binding** (`dts/bindings/input/gpio-keys.yaml`):
  - parent properties: `debounce-interval-ms` (int, default 30), `polling-mode` (boolean), `no-disconnect` (boolean)
  - child properties: `gpios` (required), `label`, `zephyr,code`
- **`polling-mode`:** a devicetree `boolean` is true only if it's written, so **the default is interrupts**. It's a parent property (applies to all keys), and it's only read by the gpio-keys input driver, which isn't built here. My first guess ("polling is enabled by default") was wrong.

## What I found in build output (zephyr.dts / .config / generated headers / map)
**Overlay → build map:**

| Overlay | `zephyr.dts` | `devicetree_generated.h` | `gpio_dt_spec` |
|---|---|---|---|
| `my-button = &button1;` | same | `DT_N_ALIAS_my_button` → `DT_N_S_gpio_keys_S_user_button` | which node |
| `&gpio0` | `&gpio0` | `…_PH` → `DT_N_S_soc_S_gpio_S_gpio_60004000` | `.port` |
| `5` | `0x5` | `…_VAL_pin 5` | `.pin` |
| `(GPIO_ACTIVE_LOW \| GPIO_PULL_UP)` | `0x11` | `…_VAL_flags 17` | `.dt_flags` |
| `label = "User Button"` | same | `…_P_label "User Button"` (exists because the binding allows it) | — |

- The board already describes its BOOT button: `aliases { sw0 = &button0; }`, `buttons { compatible = "gpio-keys"; button0: button_0 { gpios = <&gpio0 0 0x11>; label = "BOOT Button"; }; }`.
- Startup print on hardware: `Button: pin 5, dt_flags 0x11 (ACTIVE_LOW | PULL_UP)`, `LED: pin 13, dt_flags 0x00`.

**Why GPIO5 (7-check method, as in `lessons/01_getting-started/gpio-analysis.md`):**
- not flash/PSRAM/USB/console
- not a strapping pin
- `gpio0` cell 5
- ADC1 ch4, RTC GPIO 5 (deep-sleep wake), touch pad 5
- the board's pinctrl names it for I2C1 SCL / I2S0 WS / TWAI TX, but **all three are disabled**
- reused as a button in lessons 06 and 09

## Go deeper: breaking the overlay three ways
| Break | Fails at | Signature | Meaning |
|---|---|---|---|
| alias typo | C compile (`main.c.obj`) | `DT_N_ALIAS_<name>_P_gpios_IDX_0_VAL_pin undeclared` (the "did you mean …button_0…" hint is a red herring) | the alias **isn't in the devicetree**. dtc doesn't know which aliases C wants |
| missing `#include <zephyr/drivers/gpio.h>` | C compile | `incomplete type 'struct gpio_dt_spec'`, `implicit declaration of function 'GPIO_DT_SPEC_GET'`, `'GPIO_INPUT' undeclared` | the **header is missing**. `DT_ALIAS` still expanded (`devicetree.h` comes via `kernel.h`), but nothing turned the node ID into a struct |
| wrong flags | **nothing, it builds** | no `PULL_UP` → random presses (floating input). `ACTIVE_HIGH \| PULL_UP` → stable but **inverted** (LED on when released) | only real hardware shows it |

- `PULL_UP` = electrical stability; `ACTIVE_LOW/HIGH` = interpretation.

## Go deeper: extending the board's node deliberately (`workspace/build/exp-dtmerge/`)
- `&{/buttons} { button1: user-button { … }; };` puts my button **inside the board's `/buttons`**, next to the BOOT button, with the parent's properties unchanged.
- Generated: `DT_N_S_buttons_FOREACH_CHILD(fn) fn(…button_0) fn(…user_button)` and `DT_N_ALIAS_my_button → DT_N_S_buttons_S_user_button`.
- **When to extend vs add:** extend when code iterates over *all* children of that node (`DT_FOREACH_CHILD`, or the gpio-keys driver handling every key). Add a separate node when you want no side effects on board nodes (what I did).

## Bugs I hit → root cause → fix
- **dtc syntax error:** nodes closed with `}` instead of `};` → every node ends with `};`.
- **Duplicate label `button0`:** the board already uses it (BOOT button) → `button1`.
- **Alias `my-btn` vs `my-button`:** didn't match the lesson and tools → `my-button`.
- **No switch at first:** the options were the BOOT button (GPIO0, a strapping pin, so don't hold it at reset) or a jumper to GND. I got a real switch in the end.
- **"Not ready" printed but carried on** (my first `main.c`) → `return -ENODEV`. Also checked every configure/get/set return value.
- **Stale build after the flag experiments:** the build folder still had `dt_flags 0x10` while the overlay was back to `0x11` → rebuild and reflash. Check `zephyr.dts` and the startup print after every overlay change.
- **Board not connected** (`No ESP32 serial port found`) → plug in the UART port.

## Challenge: my approach vs the course solution
Challenge: the button controls the LED (pressed = on). ✅ Verified on hardware.

| | Mine | Course (`04_solution_button_led`) | Better |
|---|---|---|---|
| Overlay node | new `/gpio-keys` | `buttons` = **same path as the board's** → merged: its `polling-mode` and `debounce-interval-ms` now apply to the board's BOOT button node, and labels `button0` and `button_0` sit side by side | mine: no side effects |
| Read error | returns before touching the LED | `gpio_pin_set_dt(&led, state)` runs with a **negative error code**, so the LED turns **on** on a read failure | mine (a course bug) |
| Console | prints on change | prints every 100 ms | mine |
| Errors | prints the code, returns it | `return 0` silently | mine |
| `dt_flags` | decoded | raw hex | mine |
| LED start | `GPIO_OUTPUT_INACTIVE` | `GPIO_OUTPUT` | mine |
| Unused props | none | `debounce-interval-ms`, `polling-mode` (no driver reads them) | mine |

**Review findings (no source bugs):**
- no real debounce: the 100 ms poll hides switch bounce (now commented)
- polling latency and wake-ups (lesson 09: interrupts)
- `prj.conf` now states `CONFIG_GPIO=y`
- node names → convention `user-button`/`user-led` (lowercase, hyphens)
- `label` is descriptive only (not used for lookup since Zephyr 3.2)
- if `CONFIG_INPUT`/`CONFIG_LED` are ever enabled, those drivers will claim GPIO5/GPIO13

## Self-check (README §7)
- **Why is the button `GPIO_ACTIVE_LOW`, and what does `gpio_pin_get_dt` return when pressed?** Pressing connects the pin to GND (0 V), so "active" is the low level. `gpio_pin_get_dt()` returns the **logical** level, so it returns **1** when pressed.
- **Label vs alias?** A label is a devicetree-internal nickname (`button1:` → `&button1`). An alias is a name exported to C (`my-button` → `DT_ALIAS(my_button)`).
- **Where is the final merged devicetree written?** `workspace/build/04_button_led/zephyr/zephyr.dts`.

## Open questions
- [ ] Software debounce: require N consecutive equal reads before accepting a change (needed once polling is fast, or with interrupts).
- [ ] What would change if `CONFIG_INPUT=y`: input events with `zephyr,code`, debounce from the devicetree, interrupts by default.

## Next time / ideas
- Lesson 05 (Devicetree Bindings): write my own binding. The `gpio-keys.yaml` reading is the warm-up.
- Lesson 09: replace polling with `gpio_pin_interrupt_configure_dt()` + a callback.
