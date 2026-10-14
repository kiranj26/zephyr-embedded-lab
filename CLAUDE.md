# Zephyr Embedded Lab — project memory for Claude Code

## Who I am and what this repo is
I'm learning Zephyr RTOS by following Shawn Hymel's **Introduction to Zephyr** (DigiKey, 12 parts).
This is my personal lab. The course repo is a git submodule at `course/` (read-only reference).
My own code lives in `workspace/`. Lesson guides live in `lessons/NN_*/README.md`.

- Host: macOS. Docker Desktop is installed. Docker on macOS **cannot see USB**, so the
  container builds and the Mac flashes and monitors.
- Board: **ESP32-S3-DevKitC-1 with ESP32-S3-WROOM-1-N16R8** (16 MB quad flash, 8 MB **octal** PSRAM).
- Board target: `esp32s3_devkitc/esp32s3/procpu` (the course's target). My own board for lesson 12 is
  `s3_n16r8/esp32s3/procpu` (in `workspace/boards/custom/s3_n16r8`).
- Zephyr: the commit pinned in `course/Dockerfile.espressif` (2024-era, Zephyr SDK 0.16.8). **LVGL is v8 API.**
  Trust `/opt/toolchains/zephyr` inside the container over the latest online docs.

## Golden rules (always follow)
1. **Never edit anything under `course/`.** A hook blocks it. To change course code, copy it into `workspace/apps/`.
2. **Build, flash, and observe only through `make`:**
   - `make build APP=<prefix>` builds in the container (`APP=course/<name>` builds the reference).
   - `make flash APP=<prefix>` flashes from the Mac with esptool.
   - `make capture SECS=8` resets the board and prints serial output, then exits. Use this, **never `make monitor`**, which is interactive and never returns.
   - Builds land in `workspace/build/<app>` (course apps in `workspace/build/course-<app>`).
3. **Check pins before wiring advice.** Never suggest GPIO 26–37 (flash + octal PSRAM), 19/20 (USB), or 43/44 (console).
   Run `python3 scripts/check_pins.py <overlay>` after editing devicetree files (a hook also does this).
4. **Tutor mode by default.** I'm here to learn. When I'm working on a lesson's own app or challenge, give hints,
   questions, and pointers to docs or source first. Show full solutions only if I say "show solution" or "just fix it".
   Comparing my code with `course/workspace/apps/NN_solution_*` is fine **after** I've attempted it.
5. **Secrets:** Wi-Fi credentials go in `src/secrets.h`, which is git-ignored. Never print or commit them.
6. **Explain the why.** When something fails, show me where in the generated output to look:
   `build/.../zephyr/zephyr.dts`, `.config`, `devicetree_generated.h`, or `zephyr.map`.

## Map
| Path | What |
|---|---|
| `lessons/NN_*/README.md` | Per-episode guide: watch notes, steps, challenge, self-check |
| `docs/02-course-map.md` | Playlist ↔ blog posts ↔ course folders ↔ challenges |
| `docs/01-hardware.md` | Pin map, wiring per lesson, forbidden pins |
| `docs/05-known-issues.md` | Bugs and traps in the course repo |
| `workspace/apps/` | My apps (starters with TODOs), `_template/` for new apps |
| `workspace/modules/_templates/` | Kconfig-module and sensor-driver skeletons |
| `workspace/boards/custom/s3_n16r8/` | My custom board definition |
| `PROGRESS.md` | My progress tracker. Update it when I finish things. |

## How to help me well
- When I start a lesson (`/lesson NN`), read `lessons/NN_*/README.md` and check hardware/wiring first.
- When a build fails, delegate to the **build-doctor** agent. For devicetree questions use **devicetree-navigator**,
  for Kconfig use **kconfig-detective**, for threads/ISRs use **rtos-analyst**.
- Prefer small, verifiable steps: edit, `make build`, `make flash`, `make capture`, read the output.
