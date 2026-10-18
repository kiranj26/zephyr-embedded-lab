# Notes — 05_dt-bindings

Started: 2026-10-17 · Finished: 2026-10-18

## The idea in my own words (3–5 sentences)
A binding is the rulebook for a `compatible`: it says which properties a node and its children must or may have, their types and allowed values, and its description explains what they mean on this chip. Bindings validated my nodes all along; I only had to read them once a peripheral had children, enums and chip-specific meanings, like the ESP32 ADC (gain = attenuation) and LEDC (channel, timer, pinctrl). The pattern is always provider plus consumer: `&adc0`/`channel@5` and `&ledc0`/`channel@0` describe the hardware, and `zephyr,user` says which input and output my app uses, so the C contains no pins or numbers. The binding catches missing or malformed properties at build time, but only the driver knows what the chip can really do, so some mistakes (`ADC_GAIN_2`, a channel above 7) only show up as runtime errors, and others (mismatched channel numbers) not at all. When numbers look wrong, I read the driver: that's how I found the calibrated "raw" value and the correct mV formula.

## Key concepts
- **Bindings were always there.** Every `compatible` has a YAML binding that validated my nodes in lessons 01–04 (`gpio-leds`, `gpio-keys`, `espressif,esp32-gpio`, `…-trng`, `…-uart`). GPIO nodes needed only 1 property, so copying an example was enough. The ADC has children, enums and chip-specific meanings, so I had to read the rulebook.
- **How to read a binding:**
  - **Sections:** `description` (read it!), `compatible` (the link to nodes), `include` (inheritance), `properties` (the node itself), `child-binding` (its children).
  - **Each property:** `type`, `required`, `enum` (allowed values), `const` (fixed value).
  - **Follow every `include:`:** `espressif,esp32-adc.yaml` → `adc-controller.yaml` → `base.yaml`.
- **"Required" means the final merged node must have it, not that I must write it.** The SoC `.dtsi` already sets `unit`, `channel-count` and `#io-channel-cells`, so my overlay only enables the node and adds a channel.
- **Two kinds of failure:**

  | Mistake | Caught by | When |
  |---|---|---|
  | missing a `required` property | the binding | **build time** (devicetree step) |
  | a value in the `enum` that the chip can't do (e.g. `ADC_GAIN_2`) | the driver (`gain_to_atten()` → `-ENOTSUP`) | **runtime**: `adc_channel_setup()` fails. Seen on the board as `Could not set up ADC` |

  So: the binding checks the *format*; the driver checks the *hardware*. Always check return values.
- **Provider / consumer:** `&adc0` + `channel@5` = provider (*how* to sample). `zephyr,user { io-channels = <&adc0 5>; }` = consumer (*which* input). Same pattern as `gpio0` and `gpios = <&gpio0 13 …>`.
- **`zephyr,user`** is a special node for application data, with no binding rules. In C: `DT_PATH(zephyr_user)`.
- **Analog pins are fixed in silicon:** the ADC is not routed through the GPIO matrix, unlike digital signals, so only ADC pins work. ESP32-S3: ADC1 = GPIO1–10 (ch 0–9), ADC2 = GPIO11–20 (unusable with Wi-Fi).
- **Zephyr `adc0` = Espressif ADC1** (Zephyr counts from 0).
- **PWM:** a digital pin can't output "half", so it switches on/off fast and the eye averages it. period = one on+off cycle; pulse = on-time; duty = pulse/period. **Zephyr's PWM API is in nanoseconds** (`PWM_NSEC(x) = x`, `PWM_USEC`, `PWM_MSEC`, `PWM_HZ(f) = 1e9/f`).
- **LEDC** (the ESP32's PWM): the S3 has **8 channels (0–7)** and **4 timers (0–3)**. A timer sets the frequency; a channel picks a timer and sets the duty. The hardware generates the waveform on its own, so the CPU only changes the duty.
- **Period vs resolution trade-off** (`pwm_led_esp32.c`): ticks per period = 80 MHz / f, so the driver picks the largest resolution that fits (max 14 bits on the S3).

  | Period | Freq | Levels | Verdict |
  |---|---|---|---|
  | 20 ms | 50 Hz | 14 bits | **flicker** visible (peripheral vision, camera bands) |
  | **1 ms** | **1 kHz** | **14 bits (16 384)** | ✅ sweet spot for LEDs |
  | 1 µs | 1 MHz | ~6 bits (64) | steppy brightness |

- **pinctrl routes internal peripheral signals to pins** through the GPIO matrix. `LEDC_CH0_GPIO13` = `ESP32_PINMUX(13, ESP_NOSIG, ESP_LEDC_LS_SIG_OUT0)`. The board has no LEDC group, so I defined `&pinctrl { ledc0_default: … group1 { pinmux; output-enable; } }`. **`pinctrl-0`/`pinctrl-names = "default"` on `&ledc0` apply it** (allowed by `pinctrl-device.yaml`). Without them, the group exists but is never used (like the board's unused `i2s0_default`).
- **Channel 0 must agree in three places:** `LEDC_CH0_GPIO13` ↔ child `reg = <0>` ↔ the first `pwms` cell. A mismatch builds and runs **silently with a dark LED**.
- **The binding allows more than the chip:** LEDC `reg` enum 0–15, but the S3 has 0–7. The same lesson as `ADC_GAIN_2`.
- **Overlay macros need their header:** `PWM_HZ`/`PWM_POLARITY_NORMAL` require `#include <zephyr/dt-bindings/pwm/pwm.h>`. The ADC and pinctrl macros worked without an include because the SoC/board `.dtsi` files already include their headers. Missing it gives `devicetree error: … parse error: expected number or parenthesized expression`.

## Why GPIO6 (instead of the course's GPIO1)
- 7-check method: ADC1 **channel 5**; not strapping, flash or USB; the board's pinctrl mentions it for I2S0 BCK / TWAI RX, but both are disabled; unused by every later lesson, so it can stay wired.
- Other free ADC1 options: GPIO2 (ch1), GPIO7 (ch6).
- To avoid: GPIO3 (strapping), GPIO4/5 (buttons, L06/09), GPIO8–10 (display, L10/12).
- **Consequence:** everything that says "channel 0" in the course is **channel 5** for me. The course demo was copied to `workspace/apps/05_demo_adc_gpio6` and only its overlay changed (`channel@5`, `reg = <5>`). The C takes the channel from the devicetree.

## ADC binding: what a channel node needs
| Property | Required? | My value | Meaning on the ESP32-S3 |
|---|---|---|---|
| `reg` | ✅ | `<5>` | channel 5 = GPIO6. Must match `channel@5` and `io-channels` |
| `zephyr,gain` | ✅ (enum) | `"ADC_GAIN_1_4"` | = **11 dB attenuation**, ~0–3.1 V. Only `1`, `4_5`, `1_2`, `1_4` work |
| `zephyr,reference` | ✅ (enum) | `"ADC_REF_INTERNAL"` | internal ~1.1 V |
| `zephyr,acquisition-time` | ✅ | `ADC_ACQ_TIME_DEFAULT` | not configurable on the ESP32 |
| `zephyr,resolution` | optional | `<12>` | the S3 supports **12 only** |
| `zephyr,vref-mv` | optional | *(removed)* | **ignored with `ADC_REF_INTERNAL`**: `adc_raw_to_millivolts_dt()` asks the driver instead |

- **The node must be named `channel`**, or the ADC API macros can't find it.
- Controller: `#address-cells = <1>`, `#size-cells = <0>` (both `const`), `status = "okay"`.

## APIs / macros
| Name | What it does |
|---|---|
| `ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), knob)` | follows `io-channels`/`io-channel-names` → `{.dev, .channel_id, .channel_cfg, .resolution, …}` |
| `adc_is_ready_dt(&spec)` | the driver's init succeeded (before `main`) |
| `adc_channel_setup_dt(&spec)` | applies gain/attenuation etc. to the hardware (**this is where `-ENOTSUP` appears**) |
| `adc_sequence_init_dt(&spec, &seq)` | sets **only** `channels`, `resolution`, `oversampling`. The buffer is the caller's job |
| `adc_read_dt(&spec, &seq)` | one conversion into `seq.buffer` |
| `adc_raw_to_millivolts_dt(&spec, &mv)` | **in place**. Internal reference → `adc_ref_internal(dev)` (1100) × undoing the gain |
| `PWM_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), led)` | follows `pwms`/`pwm-names` → `{.dev = ledc0, .channel = 0, .period = 1000000, .flags = 0}` |
| `pwm_is_ready_dt(&spec)` | LEDC driver init OK (pinctrl applied) |
| `pwm_set_pulse_dt(&spec, pulse_ns)` | set the on-time in ns, keeping the devicetree period. No "setup" call: the devicetree is the configuration |

## Kconfig
- `CONFIG_ADC=y` → `CONFIG_ADC_ESP32=y` (`default y`, `depends on DT_HAS_ESPRESSIF_ESP32_ADC_ENABLED`): the two-switch rule.
- `CONFIG_PWM=y` → `CONFIG_PWM_LED_ESP32=y` (`depends on DT_HAS_ESPRESSIF_ESP32_LEDC_ENABLED`, needs `&ledc0 status = "okay"`). `CONFIG_PINCTRL=y` comes from the board.

## PWM bindings: what mattered
- `espressif,esp32-ledc.yaml`: `include: [pwm-controller.yaml, pinctrl-device.yaml, base.yaml]`. `#pwm-cells` **const 3** = `channel, period, flags`. Child: `reg` (required, the channel ID) and `timer` (required, 0–3). Its description has the pinctrl template and the "reg must match pinmux" rule.
- `pwm-controller.yaml`: `#pwm-cells` required (already set by the SoC `.dtsi`).
- `pinctrl-device.yaml`: `pinctrl-0` … `pinctrl-4` (phandles) and `pinctrl-names` (one name per state; `"default"` is applied at init).
- `pwm-leds.yaml`: `pwms` required; recommends a period between 100 ns and 50 ms. I used `zephyr,user` instead (consistent with the ADC, and no LED driver can claim the pin).
- Generated: `DT_N_S_zephyr_user_P_pwms_IDX_0_VAL_channel 0`, `…_VAL_period 1000000`, `…_VAL_flags 0`.

## What I found in build output / driver source
- **The ESP32 "raw" value isn't raw** (`drivers/adc/adc_esp32.c`): hardware code → `esp_adc_cal_raw_to_voltage()` (factory eFuse calibration) → ÷4 (gain ¼) → `(mV << 12) / 1100`. So **true_mV = raw × 4400 / 4096**.
- **Measured on my 10 kΩ pot (103):**

  | Knob | Raw | Course demo (`vref-mv 3894`) | My app (`adc_raw_to_millivolts_dt`) |
  |---|---|---|---|
  | left | 0 | 0 mV | **0 mV** |
  | middle | ~1476 | ~1404 mV | **~1585 mV** |
  | right | 2934 | 2789 mV | **3151 mV** (predicted 3152 ✅) |

- The course's `zephyr,vref-mv = <3894>` makes its printed mV about **11% low**. It's a magic number; the driver source explains the correct value.
- Behaviour: a **dead zone** near 0 V, about ±10 counts of noise with the knob still, linear in the middle.
- Generated: `DT_N_S_zephyr_user_P_io_channels_IDX_0_PH → DT_N_S_soc_S_adc_60040000`, `…_VAL_input 5`, `…io_channel_names_IDX_0 "knob"`.
- The `zephyr,user` approach needs **no aliases**: C finds the node by path (`DT_PATH(zephyr_user)`).

## Bugs I hit → root cause → fix
- **`sample_buffer` never written** (compiler: `may be used uninitialized`) → `adc_sequence_init_dt()` doesn't set `.buffer`, and the struct was uninitialised (stack garbage) → `struct adc_sequence seq = { .buffer = &sample, .buffer_size = sizeof(sample) };` (designated initialiser zeroes the rest).
- `adc_sequence_init_dt()` inside the loop and unchecked → once, before the loop, checked.
- Mixed `adc_read(dev, …)` with `_dt` calls → `adc_read_dt()`.
- `adc_raw_to_millivolts_dt()` unchecked → checked; prints the error instead of raw-as-mV.
- `return -1` → `-ENODEV` / `ret`.
- `ADC_DT_SPEC_GET` expects a node with **`io-channels`** (a consumer), not the channel node → `zephyr,user`.
- **Board disconnected mid-session** (`port is busy or doesn't exist`) → replug the UART cable.
- **`devicetree error: overlay:18 (column 26): parse error`** → `PWM_HZ` unknown → `#include <zephyr/dt-bindings/pwm/pwm.h>` at the top of the overlay.
- **`pwm_is_ready_dt(&pwm_channel)`** → no such variable; the spec is `led`.
- **`CONFIG_PWM` missing** at first → added (otherwise link errors on `pwm_*`).
- **A breathe build once failed with `'__device_dts_ord_8' undeclared`**, and the identical rebuild passed. Most likely two builds wrote the same build folder at once. Not reproducible; if it happens, rebuild.
- **`APP=05_pwm_Knob` (capital K)** → macOS is case-insensitive, so it found `05_pwm_knob` but made a second build folder `build/05_pwm_Knob`. Use lowercase.

## Challenge: my approach vs the course solution
**Task:** control the LED's brightness with the potentiometer using PWM. ✅ Verified on hardware (`DEMO_PWM_KNOB.mp4`).

**My design:**
- **Consumer in `zephyr,user`** for both: `io-channels` named `knob` and `pwms` named `led`. C uses `…_GET_BY_NAME(DT_PATH(zephyr_user), …)`.
- **Mapping from mV, not raw:** `pulse = period × mV / 3300`, a physical meaning ("fraction of 3.3 V"). Full right = 3151/3300 = **95%** duty. Clamped to `[0, period]`.
- **Overflow:** 1 000 000 ns × 3300 = 3.3e9, too big for 32-bit signed maths → `(uint64_t)period_ns * mv / 3300`. Casting one operand promotes the whole multiply.
- **Linear** (not gamma), so the middle (48%) looks brighter than "half" to the eye.
- Update every 50 ms; one line per sample: raw / mV / pulse / duty.

**Measured:**

| Knob | mV | pulse | duty |
|---|---|---|---|
| left | 0 | 0 ns | 0% |
| 1100 mV | 1100 | 333 333 ns | 33% (exactly ⅓) |
| middle | ~1575 | ~477 000 ns | ~48% |
| right | 3151 | 954 848 ns | 95% |

Smooth in both directions, with no jumps (so the 64-bit maths works).

**Comparison with `05_solution_pwm_knob` (/review 05):**

| | Mine | Course | Better |
|---|---|---|---|
| PWM period | `PWM_HZ(1000)` = 1 ms → **14 bits** (16 384 levels) | `1000` = **1 µs = 1 MHz** → ~6 bits (64 levels): the self-check trap | mine |
| Mapping | mV / knob-full-scale-mv (from the devicetree) → reaches 100% | raw / 4096 → **max 72%** (calibrated raw tops out at 2934), float maths | mine |
| mV | `adc_raw_to_millivolts_dt` (correct) | `vref-mv = 3900` read but unused for brightness | mine |
| ADC read error | sleep and retry; LED off after 10 failures | `continue` with **no sleep**: spins and floods | mine |
| Console | prints only when duty changes ≥ 1% | every **10 ms = 100 lines/s** | mine |
| Setup errors | real codes | `return 0` silently | mine |
| LED description | `pwms` in `zephyr,user` | **`pwm-leds` node + alias** | course is more idiomatic: it says "there's an LED here", portable, works with the LED API |
| ADC API | `adc_dt_spec` + `io-channels` | manual `DEVICE_DT_GET` + `ADC_CHANNEL_CFG_DT` | mine (less code, fewer mistakes) |

**Review fixes applied:**
- print on change (≥ 1%) instead of 20 lines/s
- a PWM/ADC failure no longer returns from `main()` (which would freeze the LED at its last duty). Errors are counted and rate-limited, and after 10 in a row the LED goes to a safe state (off)
- full scale moved into the overlay: `knob-full-scale-mv = <3150>` in `zephyr,user` (no binding, so custom properties are allowed), read with `DT_PROP(DT_PATH(zephyr_user), knob_full_scale_mv)`. Full right now reaches 100%
- `prj.conf` comments; `int16_t` sample explained; `%u` for unsigned
- checked: the SoC `.dtsi` does **not** set `#address-cells`/`#size-cells` on `adc0`/`ledc0`, so the overlay lines are required. The S3 LEDC max is 14 bits (`SOC_LEDC_TIMER_BIT_WIDTH 14`)

## Go deeper (2 done)
- [x] **Why `zephyr,vref-mv = <3894>/<3900>` with `ADC_GAIN_1_4`?** The ESP32 driver returns a *calibrated* value scaled to its 1100 mV internal reference, so true mV = raw × 1100 × 4 / 4096 = **raw × 4400 / 4096**. 3894/3900 are trial-and-error constants that read about 11% low. With `ADC_REF_INTERNAL`, `adc_raw_to_millivolts_dt()` ignores `vref-mv` and asks the driver (the binding says the property isn't needed). Measured: 3151 mV vs the course's 2789 at full right.
- [x] **Breathing LED** (`DEMO_BREATHE` in `main.c`): 1 s up, 1 s down, 10 ms steps, `pulse = period × (i/steps)²` (gamma 2, 64-bit maths). Squaring spends more of the ramp at low duty, where the eye is most sensitive, so the fade looks even instead of rushing at the bottom.
- [ ] Multimeter calibration (optional; the part A data is ready).

## Self-check (README §7)
- **Where do you look to know which properties an ADC channel node needs?** The binding: `dts/bindings/adc/espressif,esp32-adc.yaml` and its `include` `adc-controller.yaml` (`child-binding`). Check `zephyr.dts` for what the SoC already sets.
- **What does pinctrl do on ESP32?** It routes a peripheral's internal signal (here LEDC channel 0's output) to a physical pin through the GPIO matrix, and sets the pad (output-enable, pulls). The device applies it via `pinctrl-0` + `pinctrl-names = "default"`.
- **Units of the period cell in `pwms = <&ledc0 0 1000 …>`?** **Nanoseconds**, so `1000` = 1 µs = 1 MHz, which is far too fast for a smooth dimmable LED (~64 levels). Use `PWM_MSEC(1)` / `PWM_HZ(1000)`.

## Open questions
- [ ] Multimeter check of the wiper at full right: is it 3.15 V, matching `adc_raw_to_millivolts_dt()`?
- [ ] `zephyr,oversampling`: does the ESP32 driver support it, to reduce the noise?

## Next time / ideas
- Lesson 06: write my own binding and driver (MCP9808, I2C). Today I learned to *read* bindings; next I write one.
- Possible follow-up: describe the LED with a `pwm-leds` node instead of `zephyr,user` (portable, works with the LED API).
