# Notes — 05_dt-bindings

Started: 2026-10-17 · Finished: _(in progress: part A ADC done; part B PWM, challenge, go-deeper, video still to do)_

## The idea in my own words (3–5 sentences)
_TODO at the end of the lesson._

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

## Kconfig
- `CONFIG_ADC=y` → `CONFIG_ADC_ESP32=y` (`default y`, `depends on DT_HAS_ESPRESSIF_ESP32_ADC_ENABLED`): the two-switch rule.

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

## Challenge: my approach vs the course solution
_TODO: part B (PWM via LEDC + pinctrl, knob → brightness), then `/review 05`._

## Self-check (README §7)
- **Where do you look to know which properties an ADC channel node needs?** The binding: `dts/bindings/adc/espressif,esp32-adc.yaml` and its `include` `adc-controller.yaml` (`child-binding`). Check `zephyr.dts` for what the SoC already sets.
- **What does pinctrl do on ESP32?** _TODO (part B)._
- **Units of the period cell in `pwms = <&ledc0 0 1000 …>`?** _TODO (part B)._

## Open questions
- [ ] Multimeter check of the wiper at full right: is it 3.15 V, matching `adc_raw_to_millivolts_dt()`?
- [ ] `zephyr,oversampling`: does the ESP32 driver support it, to reduce the noise?

## Next time / ideas
- Part B: the LEDC pinctrl group (`LEDC_CH0_GPIO13`), `&ledc0` + `channel0@0`, a `pwm-leds` consumer, `CONFIG_PWM`.
