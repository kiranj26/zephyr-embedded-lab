# Lesson 05 — Devicetree Bindings

**Video:** Part 5 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Devicetree Bindings](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-5-devicetree-bindings) ·
**Course code:** `05_demo_adc`, `05_solution_pwm_knob` ·
**Your starter:** `workspace/apps/05_pwm_knob` ·
**Hardware:** 10k pot wiper→GPIO1; LED GPIO13 _(my build: wiper → **GPIO6** = ADC1 channel 5)_

> Bindings are the contract between devicetree and drivers. Reading them lets you configure any peripheral without a tutorial.

## My results: demo video

Running on my ESP32-S3-DevKitC-1 (N16R8), from [`workspace/apps/05_pwm_knob`](../../workspace/apps/05_pwm_knob/src/main.c) with its [overlay](../../workspace/apps/05_pwm_knob/boards/esp32s3_devkitc.overlay).

**[▶ DEMO_PWM_KNOB.mp4](DEMO_PWM_KNOB.mp4)** (29 s): turning the potentiometer smoothly changes the LED's brightness, from off at the left to full at the right.

| | |
|---|---|
| Knob | 10 kΩ pot, wiper → **GPIO6** = ADC1 channel 5, `ADC_GAIN_1_4` (11 dB), internal reference |
| LED | GPIO13 → 330 Ω → LED → GND, driven by **LEDC channel 0** through a pinctrl group (`LEDC_CH0_GPIO13`), period 1 ms (1 kHz) |
| Mapping | linear: `pulse = period × mV / 3300` (64-bit multiply), clamped to the period |
| Serial | `Raw: 1476  mV: 1585  pulse: 480303 ns  duty: 48%` … full right: `Raw: 2934  mV: 3151  pulse: 954848 ns  duty: 95%` |

Write-up: [notes.md](notes.md) (reading bindings, the ESP32 calibrated "raw" value, pinctrl, PWM period vs resolution, comparison with the course solution)

## 1. Before you watch (5 min)
- Run `/lesson 05` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 05` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [x] How a binding YAML declares required/optional properties
- [x] ADC channel child nodes and `zephyr,*` properties
- [x] `ADC_CHANNEL_CFG_DT`, `adc_sequence`, `adc_read`
- [x] PWM via the ESP32 LEDC peripheral + pinctrl

## 3. Run the reference (read-only)
```sh
make run APP=course/05_demo_adc
make run APP=course/05_solution_pwm_knob
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/05_pwm_knob`
1. Find the ADC binding(s) in `/opt/toolchains/zephyr/dts/bindings/adc/` and list required props
2. Write the ADC part of the overlay; print raw + mV
3. Add LEDC PWM (pinctrl group + pwm-leds node + alias `led-0`)
4. Map the knob to brightness

## 5. Challenge (from the video)
Control LED brightness with the potentiometer using PWM. Course solution: `05_solution_pwm_knob`.

Stuck? `/hint 05 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 05`.

## 6. Go deeper (pick at least two)
- [ ] Use `adc_raw_to_millivolts_dt()` and calibrate against a multimeter
- [x] Breathing LED (no knob) with smooth fade
- [x] Why does the course use `zephyr,vref-mv = <3894>` / `<3900>` with `ADC_GAIN_1_4`? Research ESP32 ADC attenuation

## 7. Self-check (answer in your notes without looking)
- Where do you look to know which properties an ADC channel node needs?
- What does pinctrl do on ESP32?
- Units of the period cell in `pwms = <&ledc0 0 1000 ...>`?

## Done when
- [x] Reference runs on my board
- [x] My version in `workspace/apps/05_pwm_knob` runs and I can explain every line
- [x] Challenge attempted before peeking, then reviewed
- [x] Two "go deeper" items done
- [x] `notes.md` written (`/notes 05`), `PROGRESS.md` updated (`/progress`), committed
