# boards/

Out-of-tree board definitions (Zephyr hardware model v2). `scripts/build.sh` passes `-DBOARD_ROOT=/workspace`
automatically when `BOARD` names a board found here.

- `custom/s3_n16r8/` — **my** ESP32-S3-DevKitC-1 (N16R8) with my lesson parts baked in.
  Build: `BOARD=s3_n16r8/esp32s3/procpu make build APP=12`

The course's own boards (`nyan`, `my_board`) live in `course/workspace/boards/custom/` (BOARD_ROOT=/course).
