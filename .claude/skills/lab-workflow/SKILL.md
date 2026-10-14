---
name: lab-workflow
description: How to build, flash, capture serial output, and organize code in the Zephyr Embedded Lab repo (Docker container + macOS host). Use whenever you need to compile, flash, run, or observe firmware, or create/locate apps, builds, notes, and progress files.
---
# Zephyr Embedded Lab workflow

## Architecture
- Container `zephyr-lab` (image `env-zephyr-espressif`, built from `course/Dockerfile.espressif`).
  - `/workspace` = repo `workspace/` (read-write), `/course` = repo `course/workspace/` (read-only)
  - Zephyr at `/opt/toolchains/zephyr`, modules at `/opt/toolchains/modules`, SDK `/opt/toolchains/zephyr-sdk-0.16.8`
- macOS host: flashing (esptool) and serial (pyserial) through `venv/`. Docker on Mac cannot see USB.

## Commands (run from repo root)
| Goal | Command |
|---|---|
| Build my app | `make build APP=04` (prefix match in `workspace/apps`) |
| Build course reference | `make build APP=course/04_demo_button` |
| Custom board | `BOARD=s3_n16r8/esp32s3/procpu make build APP=12` (BOARD_ROOT auto-detected) |
| Extra CMake args | `scripts/build.sh 03 -DCONFIG_SAY_HELLO=y` |
| Flash | `make flash APP=04` |
| Observe output (non-interactive) | `make capture SECS=8` ← **use this, never `make monitor`** |
| Kconfig menu (user only, interactive) | `make menuconfig APP=03` |
| Health check | `make doctor` |
| Pin check | `python3 scripts/check_pins.py <overlay>` |

Build output: `workspace/build/<app>/zephyr/` → `zephyr.bin`, `zephyr.elf`, `zephyr.dts`, `.config`, `zephyr.map`.
The overlay `boards/<board-name>.overlay` and `boards/<board-name>.conf` inside an app are passed automatically.

## Running commands inside the container
`docker exec zephyr-lab bash -c 'source /opt/venv/bin/activate && source /opt/toolchains/zephyr/zephyr-env.sh && <cmd>'`
Useful: `west build -d /workspace/build/<app> -t rom_report`, `-t ram_report`, `grep -rn` across `/opt/toolchains/zephyr`.

## Interpreting capture output
- `*** Booting Zephyr OS build ...` = the app started. No banner = wrong port, a bad flash, or a console moved to USB (lesson 7).
- `Guru Meditation`/`FATAL EXCEPTION` + PC address → `xtensa-espressif_esp32s3_zephyr-elf-addr2line -e zephyr.elf <addr>`.
- Garbage characters = wrong baud (console is 115200).

## Repo conventions
- New app: `make new-app NAME=NN_name`. Notes: `make notes N=NN`. Progress: `PROGRESS.md`.
- Never modify `course/` (a hook blocks it). Secrets go in `src/secrets.h` (git-ignored).
