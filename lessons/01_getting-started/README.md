# Lesson 01 — Getting Started – Installation and Blink

**Video:** Part 1 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Getting Started – Installation and Blink](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-1-getting-started-installation-and-blink) ·
**Course code:** `01_blink` ·
**Your starter:** `workspace/apps/01_blink` ·
**Hardware:** LED + 330 Ω on GPIO13 (plus a second LED + 330 Ω on GPIO17 for the alternate demo)

> Everything later builds on this loop: Docker build → host flash → serial console. You also meet devicetree aliases and the GPIO API for the first time.

## My results: demo videos

Running on my ESP32-S3-DevKitC-1 (N16R8), from [`workspace/apps/01_blink`](../../workspace/apps/01_blink/src/main.c). The demo is picked with `#define DEMO` in `main.c`.

| Demo | Video | What it shows | Serial output |
|---|---|---|---|
| `DEMO_SOS` | [▶ DEMO_SOS.mp4](DEMO_SOS.mp4) (15 s) | LED 1 (GPIO13) blinking `· · ·  — — —  · · ·` in Morse, unit = 200 ms | `SOS #n  cycle: 6802200 us`: ideal 6.8 s + 22 sleeps × 1 tick |
| `DEMO_ALTERNATE` | [▶ DEMO_ALTERNATE.mp4](DEMO_ALTERNATE.mp4) (5 s) | LED 1 (GPIO13) and LED 2 (GPIO17) always opposite, swapping every second | `LED1 (GPIO13): 1   LED2 (GPIO17): 0` |

Write-ups: [notes.md](notes.md) (concepts, findings, bugs) · [todo2-sos-guide.md](todo2-sos-guide.md) (SOS design and timing analysis) · [gpio-analysis.md](gpio-analysis.md) (why GPIO17)

## 1. Before you watch (5 min)
- Run `/lesson 01` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 01` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [x] What Zephyr is (kernel + drivers + subsystems + board support) and why it's vendor-neutral
- [x] The Docker image: what's in /opt/toolchains (zephyr, modules, SDK) vs /workspace
- [x] The board target string `esp32s3_devkitc/esp32s3/procpu` (board / SoC / CPU cluster)
- [x] How the overlay creates `my-led` and how `GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios)` uses it

## 3. Run the reference (read-only)
```sh
make run APP=course/01_blink
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/01_blink`
1. Wire the LED (`/wire 01`)
2. Read `workspace/apps/01_blink/src/main.c` line by line; answer the `Q:` comments
3. `make run APP=01`. You should see `LED state: 1/0` every second
4. Open `workspace/build/01_blink/zephyr/zephyr.dts` and find your `leds` node and the `aliases` entry

## 5. Challenge (from the video)
No formal challenge. Do the extension TODOs in the starter (SOS pattern, second LED, timing check).

Stuck? `/hint 01 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 01`.

## 6. Go deeper (pick at least two)
- [ ] Browse `/opt/toolchains/zephyr/samples/basic/blinky` and compare it with yours
- [x] Read the doc comment for `gpio_pin_set_dt` vs `gpio_pin_set_raw`
- [x] Look at `boards/espressif/esp32s3_devkitc/` in the Zephyr tree: which files define a board?

## 7. Self-check (answer in your notes without looking)
- What three parts make up a board target string?
- Why can't the container flash the board on macOS?
- Where does `DT_ALIAS(my_led)` get its value at compile time?
- What's the difference between `printk` and `printf` here?

## Done when
- [x] Reference runs on my board
- [x] My version in `workspace/apps/01_blink` runs and I can explain every line
- [x] Challenge attempted before peeking, then reviewed
- [x] Two "go deeper" items done
- [x] `notes.md` written (`/notes 01`), `PROGRESS.md` updated (`/progress`), committed
