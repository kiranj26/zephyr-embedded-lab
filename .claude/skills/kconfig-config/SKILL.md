---
name: kconfig-config
description: How Kconfig works in this lab - configuration layering, prj.conf vs board .conf, menuconfig, diffconfig, module Kconfig files, depends on vs select, and common symbols used in the course. Use for any CONFIG_ question or configuration bug.
---
# Kconfig in the lab

**Layering (later wins):** SoC/board defaults → board `_defconfig` → app `prj.conf` → `boards/<board>.conf` (passed as
`EXTRA_CONF_FILE` by `scripts/build.sh`) → `-DCONFIG_X=y` on the command line. The result is `workspace/build/<app>/zephyr/.config`.

**Inspect:** `make menuconfig APP=NN` (interactive, for the user). `grep CONFIG_X workspace/build/<app>/zephyr/.config`.
For diffs: `python $ZEPHYR_BASE/scripts/kconfig/diffconfig .config.old .config` inside the container.

**Define your own (module):** `Kconfig` with `config MY_OPT` / `bool "prompt"` / `default n` / `depends on PRINTK` / `help ...`,
and `zephyr/module.yaml` containing `kconfig: Kconfig`. In CMake use `if(CONFIG_MY_OPT)` or `add_subdirectory_ifdef(CONFIG_MY_OPT dir)`.
In C use `#ifdef CONFIG_MY_OPT` or `IS_ENABLED(CONFIG_MY_OPT)`.

**depends on** = a gate (the value can't be y unless the dependency is y). **select** = forces another symbol on. Use sparingly.
If an assignment "doesn't stick", the build prints a warning. Find the unmet dependency.

## Symbols the course uses
| Lesson | Symbols |
|---|---|
| 3 | `TEST_RANDOM_GENERATOR`, `TIMER_RANDOM_GENERATOR`, `TEST_CSPRNG_GENERATOR`, `STACK_POINTER_RANDOM=0`, `PICOLIBC_IO_FLOAT`, `FPU` |
| 5 | `ADC`, `PWM` |
| 6 | `GPIO`, `I2C`, `SENSOR`, `MCP9808` (custom), `CUSTOM_BUTTON` (custom), `LOG`, `LOG_DEFAULT_LEVEL=4` |
| 8 | `CONSOLE_SUBSYS`, `CONSOLE_GETLINE` |
| 9 | `COUNTER` |
| 10 | `DISPLAY`, `LVGL`, `LV_*` (memory, widgets, `LV_FONT_MONTSERRAT_20`), `MAIN_STACK_SIZE=4096` |
| 11 | `WIFI`, `NETWORKING`, `NET_IPV4`, `NET_TCP`, `NET_SOCKETS`, `NET_DHCPV4`, `DNS_RESOLVER`, `NET_L2_ETHERNET`, `HTTP_CLIENT`, `ESP32_WIFI_STA_AUTO_DHCPV4`, `HEAP_MEM_POOL_SIZE` |
