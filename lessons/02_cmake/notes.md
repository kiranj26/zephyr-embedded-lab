# Notes — 02_cmake

Started: 2026-10-15 · Finished: 2026-10-15

## The idea in my own words (3–5 sentences)
CMake doesn't compile anything. It describes **targets** (libraries, executables) and how they depend on each other, then writes `build.ninja`, and Ninja does the actual compiling and linking. In a Zephyr app I don't create the executable. `find_package(Zephyr)` creates a library called `app`, and Zephyr links it with the kernel and drivers into `zephyr.elf`. My own code becomes a separate library (`say_hello`) that I bring in with `add_subdirectory` and connect with `target_link_libraries(app PRIVATE say_hello)`. Every Zephyr-facing library must also link `zephyr_interface` to inherit Zephyr's include paths, `CONFIG_*` macros and CPU flags.

## Key concepts
- **Every Zephyr build is CMake.** `west build` runs CMake (configure: devicetree, Kconfig, modules, then `build.ninja`), Ninja (compile and link), then post-build (`esptool` → `zephyr.bin`). Lesson 01 already used it.
- **Targets and relationships:** `add_library`, `add_executable`, `target_include_directories`, `target_link_libraries`. In the plain-CMake demo, 4 lines became 4 Ninja steps.
- **PRIVATE / PUBLIC / INTERFACE:** for me only / for me and anyone who links me / only for whoever links me. A header in `include/` is meant for users, so it's PUBLIC.
- **Compile vs link errors:**
  - missing header → **compile** error: `fatal error: x.h: No such file or directory`
  - missing code → **link** error: `undefined reference to 'x'`
  - My first guess ("it's a link error") was wrong. Compiling always comes first, and the build stopped at step 4/205.
- **`app` is a library, not an executable.** It's created by `zephyr_library_named(app)` in `cmake/modules/kernel.cmake:218`, inside `find_package(Zephyr)`, and output as `build/.../app/libapp.a`. That's why `find_package` must come before any `target_*(app ...)`.
- **`zephyr_interface`** is an INTERFACE target with no code, only usage requirements: 83 include dirs, defines (`-DKERNEL`, …), `-imacros autoconf.h`, and CPU flags (`-mlongcalls`). `target_link_libraries` on an INTERFACE target means "inherit its compile settings", not linking in the usual sense.
- **`-imacros autoconf.h` in every compile:** no file can forget the `CONFIG_*` values. A forgotten include would silently turn every `#ifdef CONFIG_X` false, with no error. `-imacros` keeps only the macros.
- **Modules** (hal_espressif, lvgl, mbedtls…) plug into the same CMake build via `zephyr/module.yml`. All are discovered (`zephyr_modules.txt`), but only those enabled by Kconfig are compiled. It's the two-switch idea again.

## APIs / macros
| Name | What it does | Where |
|---|---|---|
| `find_package(Zephyr …)` | Loads Zephyr's build system and creates `app` | top `CMakeLists.txt` |
| `add_subdirectory(dir)` | Runs `dir/CMakeLists.txt` (defines its targets) | CMake |
| `add_library(name STATIC srcs)` | Creates a static library target → `libname.a` | CMake |
| `target_include_directories(t PUBLIC dir)` | Adds `-I dir` for `t` (and for its consumers if PUBLIC) | CMake |
| `target_link_libraries(t PRIVATE x)` | `t` uses `x`: its code at link time, and its usage requirements at compile time | CMake |
| `zephyr_library_named(n)` | `add_library(n STATIC "")` + `PUBLIC zephyr_interface` + adds `n` to `ZEPHYR_LIBS` | `cmake/modules/extensions.cmake:453` |
| `zephyr_library_include_directories()` | `target_include_directories(<current lib> PRIVATE …)` | `extensions.cmake:517` |
| `zephyr_include_directories()` | Adds include dirs **globally** (to `zephyr_interface`) | `extensions.cmake:96` |
| `printk()` | Zephyr kernel print | `zephyr/sys/printk.h` |

## Kconfig symbols · Devicetree properties
- `CONFIG_GPIO=y`: now stated in `prj.conf` (the board defconfig sets it too).
- `CONFIG_PICOLIBC=y`, `CONFIG_PICOLIBC_USE_TOOLCHAIN=y`, `CONFIG_PICOLIBC_IO_LONG_LONG=y`: **picolibc** is the C library, so it's what provides `printf`.
- The overlay is the same as lesson 01: `my-led` → `gpio0` pin 13.

## What I found in build output (zephyr.dts / .config / generated headers / map)
- **Plain CMake demo:** `[1/4]` compile main.c, `[2/4]` compile my_lib.c, `[3/4]` archive `libmy_lib.a`, `[4/4]` link `hello_world`.
- **The `PUBLIC` → `PRIVATE` experiment** (throwaway copy): failed at `[1/4]`, `main.c: fatal error: my_lib.h`. `my_lib.c` still compiled, because PRIVATE still applies to the library itself.
- **Ninja step count:** lesson 01 = 203, lesson 02 = 205. The +2 are compiling `say_hello.c` and archiving `libsay_hello.a`.
- **`build.ninja`:** `build lib/say_hello/libsay_hello.a: C_STATIC_LIBRARY_LINKER__say_hello_ …say_hello.c.obj`
- **`zephyr.map`:** `say_hello` at `0x4200306c`, size `0xe` (14 B).
- **`rom_report`:** agrees (14 B, `0x4200306c`, `.flash.text`). The whole app is **152 B = 0.24%** of the image (`main.c` 138 B + `say_hello` 14 B). The rest is Zephyr.
- **Without `zephyr_interface`:** `main.c` compiles with 133 args, 83 include dirs, 7 defines and `-imacros autoconf.h`. `say_hello.c` compiled with **6 args and 1 include dir**, so `fatal error: zephyr/kernel.h`.

## Experiments run while I watched the video (2026-10-15)

### printf vs printk: what does printf really cost? (`workspace/build/exp-printf/`)
| `say_hello.c` | `.text` (bytes) | What got linked |
|---|---|---|
| `printk("Hello!!\n")` | 60 286 | `printk` |
| `printf("Hello!!\n")` | 60 350 (+64) | **`puts`**: GCC rewrote the `printf` of a constant string into `puts` |
| `printf("Hello!! %d\n", n)` | 60 346 (+60) | `printf` |

**Surprise:** picolibc's formatter (`vfprintf = __l_vfprintf`) is **already in the image even in the printk build**. The map shows it pulled in with no file referencing it, which is how the linker behaves when the build forces a symbol in. So on this configuration, `printf` costs only about 60 bytes extra, because its heavy part is always paid.

That also explains why the course solution (printf) was *smaller* than mine (135 532 vs 135 624 B): the difference is my extra error checks, not printf.

**Lesson:** "printk is lighter" depends on the configuration. **Measure, don't assume.**

### `zephyr_library()` vs `add_library()` (`workspace/build/exp-zlib/`)
| Variant | Result |
|---|---|
| A: `zephyr_library_named` + `zephyr_library_include_directories(include)` | ✖ **compile**: `main.c: fatal error: say_hello.h`. That helper uses **PRIVATE** |
| B: `zephyr_library_named` + `zephyr_include_directories(include)` + link to `app` | ✔ works. `libsay_hello.a`, `say_hello` in the firmware |
| C: same as B but **without** `target_link_libraries(app …)` | ✖ **link**: `undefined reference to 'say_hello'` |
| D: `zephyr_library()` (automatic name) | name = `lib..__..__..__workspace__build__exp-zlib__D_auto_name__lib__say_hello.a` |

What I learned:
- `zephyr_library_named` = `add_library(STATIC)` + **PUBLIC** `zephyr_interface` (I used PRIVATE) + registration in `ZEPHYR_LIBS`.
- **In an application, `ZEPHYR_LIBS` doesn't help.** Zephyr reads that list (`zephyr/CMakeLists.txt:937`) *during* `find_package(Zephyr)`, before my `CMakeLists.txt` adds to it. So I still need `target_link_libraries(app …)`. Auto-linking only works for libraries in Zephyr **modules** (lesson 06).
- `zephyr_append_cmake_library` is meant to warn in this situation, but it checks for a target named `zephyr_prebuilt`. This Zephyr version calls it `zephyr_pre0`, so **no warning appears**. It looks like a stale check.
- `zephyr_library()` names the library from its path relative to `ZEPHYR_BASE`. Outside the Zephyr tree that produces an ugly name, so use `zephyr_library_named(...)` there.
- `zephyr_include_directories` is **global**: every file in the build gets the path. That's convenient, but less contained than a PUBLIC include on the target.
- **Verdict:** for app-local libraries, plain `add_library` + PUBLIC include + `zephyr_interface` (what I did) is clear and contained. The `zephyr_library*` helpers belong in modules.

## Bugs I hit → root cause → fix
- **`target_link_libraries(my_lib …)` in my library file:** copy-paste leftover from the demo; that target doesn't exist. → Use the target this file creates (`say_hello`).
- **The library file was never read:** without `add_subdirectory(lib/say_hello)`, CMake never enters the folder. → TODO 1, placed before the link.
- **The `make menuconfig` / `make` "No rule" error from lesson 01** is the same rule: `make` targets run on the Mac, not in the container.

## Challenge: my approach vs the course solution
Both are correct. A blink plus a static library linked to `app`. Meaningful differences:

| | Mine | Course | Better |
|---|---|---|---|
| Target name | `say_hello` (matches folder and header) | `hello_lib` | mine: one name everywhere |
| Include guard | `SAY_HELLO_H_` / `SAY_HELLO_H_` | `SAY_HELLO_H_` / `SAY_HELLO_H` (**mismatch: the guard doesn't work**) | mine |
| Prototype | `void say_hello(void)` | `void say_hello()` (unspecified args in C) | mine |
| Print | `printk` | `printf` (picolibc). On this config only about 60 B more, see above | either; `printk`/`LOG_*` is the Zephyr norm |
| Error handling | separate checks, every return code checked, returns the real error | checks codes, returns 0 silently | mine (after cleanup) |

Review cleanup applied: TODO scaffolding removed, own header first, unused `<stdio.h>` removed, C++ guards and a doc comment, `\n` instead of `\r\n` (the Zephyr console adds `\r`), `CONFIG_GPIO=y` stated.

## Self-check (README §7)
- **What does PUBLIC on `target_include_directories` do for consumers?** It passes the include path on: anything that links the library also gets `-I <dir>`, so it can `#include` the library's headers. PRIVATE would keep it to the library itself, as the experiment showed.
- **What is the `app` target and who creates it?** A static library holding all application code. Zephyr creates it with `zephyr_library_named(app)` in `kernel.cmake`, during `find_package(Zephyr)`, and links it into `zephyr.elf`.
- **Name the 4 broad stages `west build` runs:**
  1. **Configure** (CMake: board, devicetree → `zephyr.dts`/`devicetree_generated.h`, Kconfig → `.config`/`autoconf.h`, modules)
  2. **Generate** (`build.ninja`)
  3. **Build** (Ninja: compile and link → `zephyr.elf`)
  4. **Post-build** (`esptool` → `zephyr.bin`)

## Open questions
- [ ] Exactly which part of Zephyr's picolibc setup forces `__l_vfprintf` into every image? (Look for `vfprintf` in `cmake/` or `lib/libc/picolibc/`.)
- [ ] Use `ram_report` to see RAM per file (useful for stack sizing in lesson 08).

## Next time / ideas
- Lesson 03 (Kconfig): write my own `Kconfig` file. `autoconf.h` and `-imacros` from this lesson are how those options reach C.
- Lesson 06: build `say_hello` as a real Zephyr **module** (`module.yml`), where `zephyr_library()` auto-linking actually works.
