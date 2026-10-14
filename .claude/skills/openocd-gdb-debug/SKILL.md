---
name: openocd-gdb-debug
description: Step-through debugging of ESP32-S3 Zephyr firmware with OpenOCD on macOS and GDB inside the Docker container, including VS Code launch config, useful GDB commands, and port-3333 pitfalls. Use for lesson 7 and any crash investigation.
---
# OpenOCD (Mac) + GDB (container)

1. Plug the board's **USB** port (native USB-JTAG), not UART. Flash from the Mac as usual. GDB in a container can't flash.
2. Mac: `brew install open-ocd` (≥0.12 has ESP32-S3 support), then `make openocd` (runs `openocd -f board/esp32s3-builtin.cfg`).
   The course's README publishes `-p 3333:3333`, which blocks OpenOCD from binding. This lab's `make up` doesn't publish it.
3. Build with debug-friendly code: `scripts/build.sh 01 -DCONFIG_DEBUG_OPTIMIZATIONS=y`.
4. Container GDB: `/opt/toolchains/zephyr-sdk-0.16.8/xtensa-espressif_esp32s3_zephyr-elf/bin/xtensa-espressif_esp32s3_zephyr-elf-gdb /workspace/build/<app>/zephyr/zephyr.elf`
   - `target remote host.docker.internal:3333`, `mon reset halt`, `break main`, `continue`, `next`/`step`/`finish`,
     `info locals`, `print var`, `backtrace`, `x /1x 0x60004004` (GPIO_OUT register), `detach`.
5. VS Code: `workspace/.vscode/launch.json` → "Debug ESP32-S3". Enter the ELF path `/workspace/build/<app>/zephyr/zephyr.elf`.
6. Crash triage without JTAG: copy the PC/backtrace addresses from `make capture` and run `addr2line -e zephyr.elf <addr>`
   using the same toolchain prefix.
