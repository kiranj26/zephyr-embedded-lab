---
name: custom-board-hwmv2
description: Creating and debugging out-of-tree Zephyr board definitions (hardware model v2) for ESP32-S3 - board.yml, Kconfig.<board>, <board>_procpu.dts, pinctrl dtsi, defconfig, board.cmake, BOARD_ROOT, and qualifier naming. Use for lesson 12 and the s3_n16r8 board.
---
# Custom board (HWMv2) for ESP32-S3

Files in `workspace/boards/custom/s3_n16r8/` (vendor dir `custom`):
| File | Purpose |
|---|---|
| `board.yml` | name, full_name, vendor, `socs: - name: esp32s3` |
| `Kconfig.s3_n16r8` | `config BOARD_S3_N16R8` + `select SOC_ESP32S3_...` and `select SOC_ESP32S3_PROCPU if BOARD_S3_N16R8_ESP32S3_PROCPU` |
| `Kconfig` | board-level defaults (e.g. `HEAP_MEM_POOL_ADD_SIZE_BOARD`) |
| `s3_n16r8_procpu.dts` | includes the SoC/module dtsi + pinctrl + partitions; chosen, aliases, LEDs/buttons/peripherals |
| `s3_n16r8-pinctrl.dtsi` | pin groups (UART0 43/44, SPI2, I2C0, ...) |
| `s3_n16r8_procpu_defconfig` | baseline Kconfig (console, serial, GPIO, clock control) |
| `board.cmake` | `include(${ZEPHYR_BASE}/boards/common/esp32.board.cmake)` |

**Naming:** target `s3_n16r8/esp32s3/procpu` → Kconfig symbol `BOARD_S3_N16R8_ESP32S3_PROCPU` (uppercase, `/` → `_`).
The course's `Kconfig.my_board` mistakenly checks `BOARD_NYAN_ESP32S3_PROCPU`. Avoid that copy-paste trap.

**Build:** `BOARD=s3_n16r8/esp32s3/procpu make build APP=12` (the script sets `-DBOARD_ROOT=/workspace`).
Course boards: `BOARD=nyan/esp32s3/procpu make build APP=course/12_demo_custom_board` (BOARD_ROOT=/course).

**Module dtsi:** check what exists for your Zephyr revision:
`docker exec zephyr-lab ls /opt/toolchains/zephyr/dts/xtensa/espressif/esp32s3/`. If there's no `n16r8` file, keep
`esp32s3_wroom_n8.dtsi` (you won't use the extra flash or PSRAM, which is fine) and select the matching `SOC_ESP32S3_WROOM_N8`.
Compare against the in-tree `boards/espressif/esp32s3_devkitc/` for anything missing.
