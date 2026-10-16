# Lesson 03 — Kconfig Tutorial

**Video:** Part 3 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Kconfig Tutorial](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-3-kconfig-tutorial) ·
**Course code:** `03_demo_kconfig`, `03_solution_kconfig_float`, `(modules/say_hello)` ·
**Your starter:** `workspace/apps/03_kconfig_random` ·
**Hardware:** none

> Kconfig decides what code is compiled in. Most 'it compiles but doesn't work' bugs are Kconfig or devicetree.

## 1. Before you watch (5 min)
- Run `/lesson 03` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 03` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [x] Why Kconfig exists (from Linux) and the menuconfig interface
- [x] Enabling a random number generator to fix a link error
- [x] `diff`/`diffconfig` between `.config.old` and `.config`
- [x] Creating a custom symbol inside a module (`zephyr/module.yaml`, `Kconfig`, `ZEPHYR_EXTRA_MODULES`)

## 3. Run the reference (read-only)
```sh
make run APP=course/03_demo_kconfig
scripts/build.sh course/03_demo_kconfig -DCONFIG_SAY_HELLO=y  # then make flash APP=course/03_demo_kconfig
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/03_kconfig_random`
1. Add TODO 1 (random print). Watch the build fail, then fix it with menuconfig (`make menuconfig APP=03`)
2. Put the symbols in `boards/esp32s3_devkitc.conf` and explain why they're board-specific
3. Copy `modules/_templates/kconfig_module` to `modules/greeting`, enable it, call `greeting_say()`
4. Use `CONFIG_GREETING_REPEAT` (an `int` symbol) in code

## 5. Challenge (from the video)
Print the random number as a float in [0, 1) with `%.3f`. Find the Kconfig that enables float printing (software vs FPU). Course solution: `03_solution_kconfig_float`.

Stuck? `/hint 03 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 03`.

## 6. Go deeper (pick at least two)
- [x] Measure flash size with and without float printing (`rom_report`)
- [x] Find out how `printk` formats floats vs `printf` (search menuconfig for CBPRINTF)
- [x] Break a `depends on` on purpose and read the warning Kconfig prints

## 7. Self-check (answer in your notes without looking)
- Order the config sources from lowest to highest priority.
- `depends on` vs `select`: which one forces a value?
- Why must `ZEPHYR_EXTRA_MODULES` be set before `find_package(Zephyr)`?

## Done when
- [x] Reference runs on my board
- [x] My version in `workspace/apps/03_kconfig_random` runs and I can explain every line
- [x] Challenge attempted before peeking, then reviewed
- [x] Two "go deeper" items done
- [x] `notes.md` written (`/notes 03`), `PROGRESS.md` updated (`/progress`), committed
