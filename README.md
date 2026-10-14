# Zephyr Embedded Lab

**A personal learning lab for Shawn Hymel's *Introduction to Zephyr* course: ESP32-S3-DevKitC-1 (N16R8) · macOS · Docker · Claude Code.**

The course teaches Zephyr RTOS in 12 episodes. This repo is the workbench around it:
- The course repo is pulled in as a read-only reference.
- Each episode has its own lesson guide and a starter app with TODOs.
- One `make` command builds in Docker, then flashes and monitors from your Mac.
- A `.claude/` setup turns Claude Code into a tutor that knows this course, this board, and this repo.

```
make bootstrap && make setup && make image && make up   # once
make run APP=01                                          # every day
```

---

## What's inside

```
zephyr-embedded-lab/
├── README.md                 ← you are here
├── CLAUDE.md                 ← project memory for Claude Code (rules, board facts, map)
├── PROGRESS.md               ← your tracker (12 lessons + capstone)
├── Makefile                  ← the single front door: build / flash / capture / doctor ...
├── .claude/
│   ├── settings.json         ← permissions + hooks
│   ├── hooks/                ← guard_course.py (course/ read-only), pin_safety.py (no PSRAM/flash pins)
│   ├── commands/             ← /lesson /build /flash-run /hint /review /explain /trace-dt /kconfig /quiz /wire ...
│   ├── agents/               ← zephyr-tutor, build-doctor, devicetree-navigator, kconfig-detective,
│   │                           firmware-reviewer, rtos-analyst, driver-mentor, hardware-safety, quiz-master
│   └── skills/               ← lab-workflow, esp32s3-n16r8-hardware, devicetree-overlays, kconfig-config,
│                               zephyr-driver-module, zephyr-kernel-rtos, lvgl-display, zephyr-wifi-net,
│                               custom-board-hwmv2, openocd-gdb-debug, course-map
├── docs/
│   ├── 00-setup-macos.md     01-hardware.md     02-course-map.md     03-learning-plan.md
│   ├── 04-workflow.md        05-known-issues.md 06-claude-code.md
│   └── cheatsheets/          west-cmake · devicetree · kernel-api · gdb-openocd
├── lessons/
│   ├── README.md             ← index
│   ├── 01_getting-started … 12_custom-board/README.md   ← per-episode guides
│   └── _notes_template.md    ← `make notes N=04` copies it to lessons/04_*/notes.md
├── scripts/                  ← build/flash/capture/doctor/... (bash 3.2-safe) + check_pins.py
├── workspace/                ← mounted at /workspace in the container (YOUR code)
│   ├── apps/
│   │   ├── _template/        ← `make new-app NAME=13_idea`
│   │   ├── 01_blink  02_hello_lib  03_kconfig_random  04_button_led  05_pwm_knob  06_my_driver
│   │   └── 07_usb_console  08_sensor_queue  09_debounce  10_lvgl_ui  11_http_client  12_board_blink
│   ├── modules/_templates/   ← kconfig_module (lesson 3), sensor_driver (lesson 6)
│   ├── boards/custom/s3_n16r8/ ← a board definition for YOUR DevKitC-1 N16R8 (lesson 12)
│   └── .vscode/launch.json   ← GDB debugging (lesson 7)
└── course/                   ← git submodule: github.com/ShawnHymel/introduction-to-zephyr (READ-ONLY)
                                 mounted at /course in the container
```

## Quick start (macOS, Docker Desktop installed)

| Step | Command | Notes |
|---|---|---|
| 1 | `git clone --recursive https://github.com/kiranj26/zephyr-embedded-lab.git && cd zephyr-embedded-lab && make bootstrap` | fetches the `course/` submodule (read-only reference) |
| 2 | `make setup` | Python 3.12 venv with esptool 4.8.1 + pyserial 3.5 (host side) |
| 3 | `make image` | builds the course's Docker image (20–40 min, once) |
| 4 | `make up` | starts container `zephyr-lab` → VS Code at http://localhost:8800 |
| 5 | `make doctor` | checks every link in the chain |
| 6 | wire an LED to GPIO13 (330 Ω) | see `docs/01-hardware.md` |
| 7 | `make run APP=01` | build → flash → monitor (quit: Ctrl+]) |

Full walkthrough: [`docs/00-setup-macos.md`](docs/00-setup-macos.md).

## Daily commands

| Command | Does |
|---|---|
| `make run APP=04` | build + flash + monitor your app (`APP` is a prefix match) |
| `make run APP=course/04_demo_button` | same, for the course's reference code (built read-only) |
| `make build / flash APP=…` | individual steps |
| `make capture SECS=10` | reset board, print output for N seconds, exit (great for scripts and Claude) |
| `make menuconfig APP=03` | Kconfig UI |
| `BOARD=s3_n16r8/esp32s3/procpu make run APP=12` | build for your custom board |
| `make list` · `make ports` · `make shell` · `make clean APP=…` | utilities |
| `make new-app NAME=13_idea` · `make notes N=04` | scaffolding |
| `make openocd` | lesson 7: OpenOCD on the Mac for GDB in the container |

## The course at a glance

| # | Episode | You'll build | Hardware |
|---|---|---|---|
| 1 | Getting Started – Installation and Blink | blink | LED |
| 2 | CMake Tutorial | blink + your own library | LED |
| 3 | Kconfig Tutorial | random numbers, float printing, your own Kconfig module | — |
| 4 | Devicetree Tutorial | button controls LED | LED, button |
| 5 | Devicetree Bindings | ADC knob → PWM brightness | pot, LED |
| 6 | Device Driver Development | custom button driver, **your own MCP9808 I2C driver** | buttons, MCP9808 |
| 7 | Debugging with OpenOCD and GDB | breakpoints; console over native USB | 2nd USB cable |
| 8 | Multithreading | threads, mutex, sensor → queue → printer | MCP9808 |
| 9 | Interrupts and Workqueues | GPIO IRQ, timers, debounced button | button |
| 10 | Graphics with LVGL and Display Drivers | UI + animation on ST7735R | 0.96" TFT |
| 11 | WiFi and IoT | DNS + sockets + HTTP client | Wi-Fi (2.4 GHz) |
| 12 | How to Create a Custom Board Definition | your own `s3_n16r8` board | — |

Details and blog links: [`docs/02-course-map.md`](docs/02-course-map.md) · plan: [`docs/03-learning-plan.md`](docs/03-learning-plan.md).

## Working with Claude Code

Run `claude` in the repo root, then try:
```
/lesson 01          # briefing + wiring + warm-up questions
/flash-run 01       # build, flash, capture, interpret
/hint 04 1          # stuck? hints, not answers
/review 04          # after your attempt: review + compare with the course solution
/trace-dt my-led 01 # how an overlay line becomes C macros
/quiz 04            # check yourself
/progress           # update the tracker
```
Claude runs in **tutor mode** (see `CLAUDE.md`): hints first, solutions only when you say "show solution".
Hooks stop it from editing `course/` or choosing pins that would collide with your board's flash/PSRAM.
More: [`docs/06-claude-code.md`](docs/06-claude-code.md).

## Your board, in one paragraph
ESP32-S3-DevKitC-1 with the **WROOM-1-N16R8** module: 16 MB flash, 8 MB **octal** PSRAM. Octal PSRAM occupies **GPIO 33–37**,
and flash uses **26–32**, so never wire those. The **UART** USB-C port flashes and carries the console. The **USB** port is native USB/JTAG,
used in lesson 7. The course target `esp32s3_devkitc/esp32s3/procpu` works as-is. Everything course-related uses safe pins.
See [`docs/01-hardware.md`](docs/01-hardware.md).

## Credits & license
Course, videos, and reference code © Shawn Hymel / DigiKey, Apache-2.0 (see `course/`).
This lab scaffold is yours to use and change. Starter code is derived from the course's structure under the same Apache-2.0 terms.
