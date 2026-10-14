# Lesson 05 — Devicetree Bindings

**Video:** Part 5 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Devicetree Bindings](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-5-devicetree-bindings) ·
**Course code:** `05_demo_adc`, `05_solution_pwm_knob` ·
**Your starter:** `workspace/apps/05_pwm_knob` ·
**Hardware:** 10k pot wiper→GPIO1; LED GPIO13

> Bindings are the contract between devicetree and drivers. Reading them lets you configure any peripheral without a tutorial.

## 1. Before you watch (5 min)
- Run `/lesson 05` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 05` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] How a binding YAML declares required/optional properties
- [ ] ADC channel child nodes and `zephyr,*` properties
- [ ] `ADC_CHANNEL_CFG_DT`, `adc_sequence`, `adc_read`
- [ ] PWM via the ESP32 LEDC peripheral + pinctrl

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
- [ ] Breathing LED (no knob) with smooth fade
- [ ] Why does the course use `zephyr,vref-mv = <3894>` / `<3900>` with `ADC_GAIN_1_4`? Research ESP32 ADC attenuation

## 7. Self-check (answer in your notes without looking)
- Where do you look to know which properties an ADC channel node needs?
- What does pinctrl do on ESP32?
- Units of the period cell in `pwms = <&ledc0 0 1000 ...>`?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/05_pwm_knob` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 05`), `PROGRESS.md` updated (`/progress`), committed
