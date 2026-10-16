# Notes — 03_kconfig

Started: 2026-10-16 · Finished: _(in progress: float challenge, go-deeper items and video still to do)_

## The idea in my own words (3–5 sentences)
_TODO after the video._

## Key concepts
- **Kconfig decides which code is compiled in.** Devicetree = what hardware exists, CMake = how to build, Kconfig = which features are in.
- **"Off" means not defined, not 0.** `CONFIG_GPIO=y` gives `#define CONFIG_GPIO 1` in `autoconf.h`. `# CONFIG_I2C is not set` gives **nothing**. My guess was `#define CONFIG_X 0`, but that would make every `#ifdef CONFIG_X` true. Zephyr's preferred check is `if (IS_ENABLED(CONFIG_X))`: both branches are compiled and type-checked, and the dead one is removed.
- **Configuration layers** (later wins), visible in every build log as `Loaded …` / `Merged …`:
  1. board `…_defconfig` (Zephyr tree, never edit)
  2. `prj.conf`: what the app needs on **any** board
  3. `boards/<board>.conf`: choices for **this board only**
  4. `-DCONFIG_X=y` on the command line → `misc/generated/extra_kconfig_options.conf`, merged last. One-off; nothing saves it
- **menuconfig edits only `build/…/zephyr/.config`**, a generated file. `make build` runs `west build -p always` (pristine), which **deletes the build folder** and regenerates `.config` from the sources. So menuconfig changes are lost unless they're copied into a `.conf` file. (`PRISTINE=never make build` keeps the folder. I lost a change to a typo: `RISTINE=never`.)
- **Workflow:** menuconfig to **explore** → `make diffconfig APP=03` to **see** → copy the `X n -> y` lines into a `.conf` to **keep**.
- **Hidden options:** menuconfig hides options whose `depends on` isn't met. **A** = show all (blocked options appear as `- -`), **C** = show `CONFIG_` names, **?** = symbol info (direct dependencies, selected by, help). **(NEW)** = not in the current `.config` (not proof that something just changed).
- **`depends on` vs `select`:** `depends on X` = a *requirement* (unmet → the option is hidden/ignored). `select X` = *forces* X on (e.g. `ENTROPY_ESP32_RNG` `select`s `ENTROPY_HAS_DRIVER`, an option with no prompt that you can't set yourself).
- **Zephyr treats Kconfig warnings as errors.** `CONFIG_GREETING_REPEAT=300` (range 1..10) → "ignored due to being outside the active range… falling back on defaults" → `error: Aborting due to Kconfig warnings` during configure (`CMakeLists.txt:6 (find_package)`). Asking for X and silently getting Y would be worse.
- **A Kconfig `int` is a compile-time constant:** `GREETING_REPEAT` 1 vs 3 → identical FLASH (135 668 B). It's just a different immediate value in the loop, with no RAM.

## APIs / macros
| Name | What it does | Header / file |
|---|---|---|
| `sys_rand32_get()` | 32-bit value from the configured random generator | `zephyr/random/random.h` |
| `sys_rand_get(buf, len)` | fill a buffer with random bytes (system call → `z_impl_sys_rand_get`) | `zephyr/random/random.h` |
| `IS_ENABLED(CONFIG_X)` | 1/0 usable in a normal C `if` (both branches compiled) | `zephyr/sys/util_macro.h` |
| `set(ZEPHYR_EXTRA_MODULES …)` | add out-of-tree modules; **must come before** `find_package(Zephyr)` | app `CMakeLists.txt` |
| `zephyr_library()` + `zephyr_library_sources()` | module library, auto-linked into Zephyr | module `CMakeLists.txt` |

## Kconfig symbols · Devicetree properties
**Random sources: two paths**

| | Path A: fake (course demo) | Path B: hardware TRNG (mine) |
|---|---|---|
| I set | `CONFIG_TEST_RANDOM_GENERATOR=y` ("allow non-random") | `CONFIG_ENTROPY_GENERATOR=y` |
| Kconfig works out | `TIMER_RANDOM_GENERATOR`, `TEST_CSPRNG_GENERATOR`, `STACK_POINTER_RANDOM=0`, `STACK_CANARIES n` | `ENTROPY_ESP32_RNG` (default y, `depends on DT_HAS_ESPRESSIF_ESP32_TRNG_ENABLED`) → `select ENTROPY_HAS_DRIVER` → `ENTROPY_DEVICE_RANDOM_GENERATOR`, `HARDWARE_DEVICE_CS_GENERATOR` |
| Build | ⚠ CMake warning "not truly random generators" | no warning |
| Use for | tests only (its help text: "should not be used in a production environment"; boards' own defconfigs must never set it) | real products |

- **Devicetree:** `chosen { zephyr,entropy = &trng0; }`, `trng0: trng@6003507c { compatible = "espressif,esp32-trng"; status = "okay"; }`. The hardware switch was already ON. Only the Kconfig subsystem switch (`ENTROPY_GENERATOR`) was off: the **two-switch rule** from lesson 01.
- The course demo's `boards/esp32s3_devkitc.conf` is **exactly** the diffconfig of toggling `TEST_RANDOM_GENERATOR`. That's how such files are made.
- **My module's Kconfig** (`workspace/modules/greeting/Kconfig`): `GREETING` (bool, default n, `depends on PRINTK`) and `GREETING_REPEAT` (int, default 1, `range 1 10`, `depends on GREETING`).

## What I found in build output (zephyr.dts / .config / generated headers / map)
- **Ninja step counts:**
  - 203 = no random generator (link error)
  - 206 = path A
  - 207 = path B (+ the ESP32 entropy driver)
  - 209 = + the greeting module (compile `greeting.c` + archive its library)
- **The link error without a generator:** `[198/203] Linking … undefined reference to 'z_impl_sys_rand_get'`. The header existed (compile OK), but no implementation was built.
- **`diffconfig` after enabling `ENTROPY_GENERATOR`:** ` ENTROPY_GENERATOR n -> y` (my change) plus `+ENTROPY_ESP32_RNG y`, `+ENTROPY_HAS_DRIVER y`, `+ENTROPY_DEVICE_RANDOM_GENERATOR y`, `+HARDWARE_DEVICE_CS_GENERATOR y`, … (consequences).
- **The course module `say_hello`:** `if(CONFIG_SAY_HELLO)` around `zephyr_library_sources`. With it off, `say_hello.c` isn't compiled. When on, it's compiled into Zephyr's own library (`zephyr/CMakeFiles/zephyr.dir/…/say_hello.c.obj`), so no `target_link_libraries(app …)` is needed.
- **Module auto-linking confirmed:** my `greeting` module (`zephyr_library()`) links without `target_link_libraries(app …)`. In app code (lesson 02 experiment) the same helper did **not** auto-link.

## Bugs I hit → root cause → fix
- **Link error `z_impl_sys_rand_get`** → no random generator enabled → `CONFIG_ENTROPY_GENERATOR=y` (or the test generator).
- **The menuconfig fix vanished after `make build`** → pristine build regenerated `.config` from the sources → put the setting in `boards/esp32s3_devkitc.conf`.
- **`RISTINE=never make build`** → typo, so the variable was ignored → `-p always` again. Spell it `PRISTINE`.
- **Captured old output after a build** → forgot `make flash`. `make capture` only resets the board.
- **`CONFIG_GREETING_REPEAT=300`** → outside `range 1 10` → the build stops with "Aborting due to Kconfig warnings".
- **My buffer loop printed `rnd` five times** → printed the wrong variable → `buffer[i]`.
- **Lab tool added:** `make diffconfig APP=NN` (`scripts/diffconfig.sh`) for a readable `.config.old → .config` comparison.

## Challenge: my approach vs the course solution
_TODO: print the random value as a float in [0, 1) with `%.3f` (`03_solution_kconfig_float`)._

## Self-check (README §7)
- **Config sources, lowest → highest priority:** board defconfig → `prj.conf` → `boards/<board>.conf` → `-DCONFIG_…` (command line).
- **`depends on` vs `select`: which forces a value?** `select`. `depends on` only allows the option when the dependency is met.
- **Why must `ZEPHYR_EXTRA_MODULES` be set before `find_package(Zephyr)`?** Zephyr discovers modules and reads their `Kconfig`/`CMakeLists.txt` while `find_package` runs. Anything set afterwards is never seen, so `CONFIG_GREETING` wouldn't exist (an "unknown symbol" error).

## Open questions
- [ ] Break a `depends on` on purpose (`CONFIG_GREETING_REPEAT=3` with `CONFIG_GREETING=n`) and compare the warning with the range one.
- [ ] When would anyone choose the timer generator on a chip with a TRNG? (Emulators/QEMU, unit tests that need repeatable runs, chips without a TRNG.)
- [ ] `XOSHIRO_RANDOM_GENERATOR`: a fast PRNG seeded from the entropy driver. When is that better than reading the TRNG every time?

## Next time / ideas
- Float challenge, then `/review 03`.
- Go-deeper: `rom_report` with and without float printing; how `printk` (CBPRINTF) vs `printf` (picolibc) handle floats.
