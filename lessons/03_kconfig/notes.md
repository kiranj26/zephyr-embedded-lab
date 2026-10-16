# Notes — 03_kconfig

Started: 2026-10-16 · Finished: _(in progress: video, own-words paragraph and /review 03 still to do)_

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
- **Two kinds of Kconfig warnings** (`scripts/kconfig/kconfig.py`):
  - **Kconfiglib warnings → abort the build.** Examples: out of `range`, unknown symbol. `CONFIG_GREETING_REPEAT=300` → "ignored due to being outside the active range" → `error: Aborting due to Kconfig warnings` during configure (`CMakeLists.txt:6 (find_package)`).
  - **"was assigned the value X but got Y" → warning only; the build passes!** Example: unmet `depends on`. `CONFIG_GREETING_REPEAT=3` with `CONFIG_GREETING=n` → "got the value ''. Check these unsatisfied dependencies: GREETING (=n)", but exit 0. The script says: *"Keep that one as just a warning for now."* (Claude first told me this would abort too. My experiment proved it doesn't.)
  - So a dropped setting can hide in a long log. Check with `make build APP=NN 2>&1 | grep -A2 "warning:"`, `.config`, or `make diffconfig`.
- **Removing `depends on` hides the problem completely:** `GREETING_REPEAT` becomes standalone, so `=3` is valid and there's no warning at all. You end up with a dead `#define CONFIG_GREETING_REPEAT 3` in `autoconf.h` that nothing uses.
- **Calculating floats ≠ printing floats.** `CONFIG_FPU` = hardware float *math*. `CONFIG_CBPRINTF_FP_SUPPORT` = the printf engine can *format* floats. Without the second, `%.3f` prints `*float*` (build passes; silent unless you read the output).
- **With picolibc, `printk` is built on picolibc's `vfprintf`** (`lib/os/printk.c`: `#ifdef CONFIG_PICOLIBC … vfprintf(&console, fmt, ap)`). So:
  - `*float*` is picolibc's placeholder from its integer-only engine `__l_vfprintf` (`PICOLIBC_IO_LONG_LONG`)
  - `CBPRINTF_FP_SUPPORT=y` makes `PICOLIBC_IO_LONG_LONG` unavailable (`depends on … !CBPRINTF_FP_SUPPORT`), so picolibc switches to `PICOLIBC_IO_FLOAT` → `__d_vfprintf`
  - it also answers lesson 02's mystery: picolibc's `vfprintf` is in *every* image because printk uses it, which is why `printf` cost only about 60 B there
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
- **Float/FPU size experiment** (`workspace/build/exp-fpcost/`, 4 pristine builds of this app):

  | Variant | `CBPRINTF_FP_SUPPORT` | `FPU` | FLASH | printf engine |
  |---|---|---|---|---|
  | A | n | n | 135 672 B | `__l_vfprintf` (integer only → `*float*`) |
  | B | **y** | n | **136 500 B (+828 B)** | `__d_vfprintf` (float) |
  | C | y | **y** | 136 500 B (±0) | `__d_vfprintf` |
  | D | n | y | 135 672 B (±0) | `__l_vfprintf` |

  - **Float printing costs only 828 B**, because the float engine replaces the integer one rather than being added on top.
  - **The FPU changed nothing:** same size and identical compiler flags for `main.c`. The code uses **double** math, which runs in software either way, and those soft-float routines (`__divdf3`, `__floatunsidf`, …) aren't even in flash: they're linked to the **ESP32-S3 mask ROM** (`esp32s3.rom.libgcc.ld`: `__divdf3 = 0x40002250`).
  - So on this chip, for this code: turn on float **printing** (`CBPRINTF_FP_SUPPORT`), and leave `FPU` alone. (`FPU_SHARING` matters later, when several threads use FP registers.)
- **Module auto-linking confirmed:** my `greeting` module (`zephyr_library()`) links without `target_link_libraries(app …)`. In app code (lesson 02 experiment) the same helper did **not** auto-link.

## Bugs I hit → root cause → fix
- **Link error `z_impl_sys_rand_get`** → no random generator enabled → `CONFIG_ENTROPY_GENERATOR=y` (or the test generator).
- **The menuconfig fix vanished after `make build`** → pristine build regenerated `.config` from the sources → put the setting in `boards/esp32s3_devkitc.conf`.
- **`RISTINE=never make build`** → typo, so the variable was ignored → `-p always` again. Spell it `PRISTINE`.
- **Captured old output after a build** → forgot `make flash`. `make capture` only resets the board.
- **`CONFIG_GREETING_REPEAT=300`** → outside `range 1 10` → the build stops with "Aborting due to Kconfig warnings".
- **`%.3f` printed `*float*`** → the printf engine (picolibc, used by printk) was the integer-only one → `CONFIG_CBPRINTF_FP_SUPPORT=y`.
- **`CONFIG_FPU=y` didn't fix it** → the FPU is about float math, not formatting.
- **The safety check blocked my experiment script** (`docker exec … bash -c "…"` can't be checked for `rm`) → call `west` directly with `docker exec -e ZEPHYR_BASE=… zephyr-lab west build …`.
- **`CONFIG_GREETING=n` + `REPEAT=3` built fine** → (1) I had commented out `depends on`, so there was no rule to break. (2) With it restored, the unmet dependency is only a *warning*, and the value is silently dropped.
- **My buffer loop printed `rnd` five times** → printed the wrong variable → `buffer[i]`.
- **Lab tool added:** `make diffconfig APP=NN` (`scripts/diffconfig.sh`) for a readable `.config.old → .config` comparison.

## Challenge: my approach vs the course solution
**Task:** print the random value as a float in [0, 1) with `%.3f`.

How I got there:
1. Uncommented `double f = (double)rnd / (UINT32_MAX + 1.0); printk("%.3f", f)`. The build **passed** but printed `Random float: *float*`.
2. Tried `CONFIG_FPU=y` first: still `*float*`. That proved the FPU isn't about printing.
3. menuconfig `/FP_SUPPORT` → `CBPRINTF_FP_SUPPORT=y`. `make diffconfig` showed the chain: ` CBPRINTF_FP_SUPPORT n -> y`, ` PICOLIBC_IO_FLOAT n -> y`, `-PICOLIBC_IO_LONG_LONG y`.
4. Result: `Random float: 0.316`. Check: 1357732849 / 2³² = 0.3161 ✓
5. Made it permanent in **`prj.conf`**, because the app's code uses `%.3f` on any board (not a board-hardware choice).

Why divide by `UINT32_MAX + 1.0` (= 2³²) and not `UINT32_MAX`? So the result is always < 1.0, giving [0, 1).

_Comparison with `03_solution_kconfig_float`: pending `/review 03`._

## Self-check (README §7)
- **Config sources, lowest → highest priority:** board defconfig → `prj.conf` → `boards/<board>.conf` → `-DCONFIG_…` (command line).
- **`depends on` vs `select`: which forces a value?** `select`. `depends on` only allows the option when the dependency is met.
- **Why must `ZEPHYR_EXTRA_MODULES` be set before `find_package(Zephyr)`?** Zephyr discovers modules and reads their `Kconfig`/`CMakeLists.txt` while `find_package` runs. Anything set afterwards is never seen, so `CONFIG_GREETING` wouldn't exist (an "unknown symbol" error).

## Go deeper (all 3 done)
- [x] **Break a `depends on`:** it only warns (see Key concepts).
- [x] **Flash cost of float printing:** +828 B, and the FPU ±0 B (table above).
- [x] **How `printk` formats floats vs `printf`:** on this configuration both go through picolibc's `vfprintf`. `CBPRINTF_FP_SUPPORT` picks the float-capable engine for both (`printk.c` → `vfprintf` under `CONFIG_PICOLIBC`).

## Open questions
- [x] Break a `depends on` on purpose: done. It's only a warning (see Key concepts).
- [ ] When would anyone choose the timer generator on a chip with a TRNG? (Emulators/QEMU, unit tests that need repeatable runs, chips without a TRNG.)
- [ ] `XOSHIRO_RANDOM_GENERATOR`: a fast PRNG seeded from the entropy driver. When is that better than reading the TRNG every time?

## Next time / ideas
- Watch Part 3 → write "the idea in my own words" → `/review 03` (compare with `03_solution_kconfig_float`) → commit, then PR.
- Lesson 04 (Devicetree) next.
