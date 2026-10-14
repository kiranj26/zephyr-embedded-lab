# Cheatsheet · west / CMake / build outputs
| Task | Command (in `make shell`) |
|---|---|
| Pristine build | `west build -p always -b esp32s3_devkitc/esp32s3/procpu -s <app> -d <build>` |
| Incremental | `west build -d <build>` |
| Kconfig menu | `west build -d <build> -t menuconfig` |
| Size reports | `west build -d <build> -t rom_report` / `ram_report` |
| List boards | `west boards \| grep esp32s3` |
| Plain CMake | `cmake -S . -B build -G Ninja && cmake --build build` |

Build outputs in `<build>/zephyr/`: `zephyr.bin` (flash this) · `zephyr.elf` (debug symbols) · `zephyr.dts` (final devicetree) ·
`.config` (final Kconfig) · `zephyr.map` (linker map) · `include/generated/**/devicetree_generated.h`.
CMake essentials: `target_sources(app PRIVATE ...)`, `add_subdirectory`, `zephyr_library()`, `zephyr_library_sources`,
`zephyr_include_directories`, `set(ZEPHYR_EXTRA_MODULES ...)` before `find_package(Zephyr)`.
