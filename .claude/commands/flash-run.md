---
description: Build, flash, and capture serial output, then interpret it
argument-hint: <app prefix> [seconds]
allowed-tools: Bash(make build:*), Bash(make flash:*), Bash(make capture:*), Read
---
For app `$1`:
1. `make build APP=$1` (stop and diagnose if it fails).
2. `make flash APP=$1` (if no port is found, tell me which USB-C port to use and to run `make ports`).
3. `make capture SECS=${2:-8}`.
4. Interpret the output: boot banner (`*** Booting Zephyr OS ...`), my prints, errors, crashes (e.g. `FATAL EXCEPTION`,
   stack overflow, `device not ready`). Tell me whether the behavior matches the lesson's expected result.
