# Lesson 04 — Devicetree Tutorial

**Video:** Part 4 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Devicetree Tutorial](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-4-devicetree-tutorial) ·
**Course code:** `04_demo_button`, `04_solution_button_led` ·
**Your starter:** `workspace/apps/04_button_led` ·
**Hardware:** LED GPIO13, button GPIO5→GND

> Devicetree describes hardware; Zephyr turns it into compile-time macros. Every peripheral in later lessons starts here.

## 1. Before you watch (5 min)
- Run `/lesson 04` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 04` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] History: Open Firmware → Linux → Zephyr (compile-time only in Zephyr)
- [ ] Nodes, labels, phandles, properties; `compatible`, `status`, `reg`
- [ ] Overlays modify the board's tree; aliases give portable names
- [ ] Bindings validate properties and generate macros

## 3. Run the reference (read-only)
```sh
make run APP=course/04_demo_button
make run APP=course/04_solution_button_led
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/04_button_led`
1. Write the overlay from scratch (aliases + gpio-leds + gpio-keys)
2. Implement main.c TODOs; `make run APP=04`
3. Print `btn.dt_flags` and decode the bits with `include/zephyr/dt-bindings/gpio/gpio.h`
4. `/trace-dt my-button 04` to see the macro chain

## 5. Challenge (from the video)
Make the button control the LED (pressed → on). Course solution: `04_solution_button_led`.

Stuck? `/hint 04 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 04`.

## 6. Go deeper (pick at least two)
- [ ] Break the overlay three ways (typo alias, missing include, wrong flags) and record each error in notes
- [ ] Read `dts/bindings/input/gpio-keys.yaml`. What does `polling-mode` change?
- [ ] Find `DT_N_ALIAS_my_button` in `devicetree_generated.h`

## 7. Self-check (answer in your notes without looking)
- Why is the button GPIO_ACTIVE_LOW, and what does `gpio_pin_get_dt` return when pressed?
- What's the difference between a label and an alias?
- Where is the final merged devicetree written?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/04_button_led` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 04`), `PROGRESS.md` updated (`/progress`), committed
