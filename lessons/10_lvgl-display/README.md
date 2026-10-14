# Lesson 10 — Graphics with LVGL and Display Drivers

**Video:** Part 10 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Graphics with LVGL and Display Drivers](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-10-graphics-with-lvgl-and-display-drivers) ·
**Course code:** `10_demo_display`, `10_solution_animation` ·
**Your starter:** `workspace/apps/10_lvgl_ui` ·
**Hardware:** ST7735R 0.96" 160×80 SPI (SCK12, MOSI10, CS9, DC18, RST8)

> Real products have UIs. You learn the display driver chain (MIPI-DBI over SPI) and LVGL's object/style model.

## 1. Before you watch (5 min)
- Run `/lesson 10` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 10` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] `chosen zephyr,display` + mipi-dbi-spi + st7735r node
- [ ] LVGL widgets: labels, lines, objects with styles
- [ ] `lv_task_handler()` loop and frame timing
- [ ] Memory/stack Kconfig for LVGL

## 3. Run the reference (read-only)
```sh
make run APP=course/10_demo_display
make run APP=course/10_solution_animation
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/10_lvgl_ui`
1. Wire the display (`/wire 10`) and run the course demo first to prove the hardware
2. Build the hello/counter/rectangle/circle UI in the starter
3. Animate the circle across the screen
4. Make styles `static`

## 5. Challenge (from the video)
Animate a shape across the screen. Course solution: `10_solution_animation`.

Stuck? `/hint 10 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 10`.

## 6. Go deeper (pick at least two)
- [ ] Show live MCP9808 temperature (lesson 8 queue → UI)
- [ ] Bar graph of the knob (lesson 5)
- [ ] Try different `madctl` values and explain the result

## 7. Self-check (answer in your notes without looking)
- Why must `lv_task_handler()` be called periodically?
- What do `x-offset`/`y-offset` correct for?
- Two causes of LVGL crashes and their Kconfig fixes?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/10_lvgl_ui` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 10`), `PROGRESS.md` updated (`/progress`), committed
