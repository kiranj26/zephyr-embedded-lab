# 04 · How this lab works under the hood

- `make build APP=X` → `scripts/build.sh` resolves `X` (prefix match) to `workspace/apps/<name>` or `course/workspace/apps/<name>`.
  It then runs, inside the container:
  `west build -p always -b $BOARD -s <src> -d /workspace/build/<name> -- -DDTC_OVERLAY_FILE=<src>/boards/<board>.overlay -DEXTRA_CONF_FILE=<src>/boards/<board>.conf [-DBOARD_ROOT=...]`
  The overlay/conf flags are only added if those files exist. `<board>` is the first part of `$BOARD` (e.g. `esp32s3_devkitc`).
- Building out-of-source (`-d /workspace/build/...`) means the read-only `/course` mount never gets written to.
- Plain CMake apps (no `find_package(Zephyr)`) are built and run natively in the container (lesson 2 demo).
- `make flash` runs esptool from `venv/` on the Mac with the same flags as the course README. `make capture` resets the board
  through RTS and prints N seconds of output. That's ideal for Claude Code, which can't use interactive terminals.
- `make up` mounts `workspace/` → `/workspace` and `course/workspace` → `/course:ro`. It does **not** publish 3333 (see known issues).
- Variables: `BOARD`, `BOARD_ROOT`, `PORT`, `BAUD`, `PRISTINE=auto` (incremental builds), `PLATFORM` (image arch).

Inside the container, `make shell` gives you a bash prompt where `west` and `ZEPHYR_BASE` are already set up.
Handy: `west build -d /workspace/build/01_blink -t rom_report | ram_report | menuconfig | guiconfig`.
