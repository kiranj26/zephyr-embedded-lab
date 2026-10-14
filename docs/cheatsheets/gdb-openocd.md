# Cheatsheet · OpenOCD + GDB (lesson 7)
Mac: `make openocd` (board on the **USB** port). Container GDB:
`/opt/toolchains/zephyr-sdk-0.16.8/xtensa-espressif_esp32s3_zephyr-elf/bin/xtensa-espressif_esp32s3_zephyr-elf-gdb /workspace/build/01_blink/zephyr/zephyr.elf`
`target remote host.docker.internal:3333` · `mon reset halt` · `break main` / `break main.c:30` · `info breakpoints` / `delete N` ·
`continue` · `next` · `step` · `finish` · `print var` · `info locals` · `backtrace` · `x /1x 0x60004004` (GPIO_OUT) · `list` · `detach` · `quit`.
Build with `-DCONFIG_DEBUG_OPTIMIZATIONS=y` for predictable stepping.
