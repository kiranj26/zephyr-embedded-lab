# Lesson 02 — CMake Tutorial

**Video:** Part 2 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [CMake Tutorial](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-2-cmake-tutorial) ·
**Course code:** `02_demo_cmake`, `02_solution_hello_blink` ·
**Your starter:** `workspace/apps/02_hello_lib` ·
**Hardware:** LED on GPIO13

> Zephyr's build system is CMake + Ninja under `west`. Knowing targets, include dirs, and linking turns build errors into readable problems.

## 1. Before you watch (5 min)
- Run `/lesson 02` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 02` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] Plain CMake: `project`, `add_library`, `add_executable`, `target_include_directories`, `target_link_libraries`
- [ ] PUBLIC vs PRIVATE propagation
- [ ] Generators: Ninja vs Unix Makefiles (`cmake -S . -B build -G ...`)
- [ ] In Zephyr the executable target already exists: it's called `app`

## 3. Run the reference (read-only)
```sh
make build APP=course/02_demo_cmake   # plain CMake: builds AND runs inside the container
make run APP=course/02_solution_hello_blink
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/02_hello_lib`
1. Finish `lib/say_hello/CMakeLists.txt` (library target + include dir + `zephyr_interface`)
2. Wire it into the top `CMakeLists.txt`
3. Uncomment the include and call in `main.c`; `make run APP=02`
4. Find your library in `workspace/build/02_hello_lib/build.ninja`

## 5. Challenge (from the video)
Add a `say_hello` library to the blink app so it prints `Hello!!` each blink (course solution: `02_solution_hello_blink`).

Stuck? `/hint 02 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 02`.

## 6. Go deeper (pick at least two)
- [ ] Why does the library need `target_link_libraries(... zephyr_interface)`? Remove it and read the error
- [ ] Run `west build -d /workspace/build/02_hello_lib -t rom_report` (in `make shell`) and find your function
- [ ] Compare `zephyr_library()` (Zephyr's helper) with plain `add_library()`

## 7. Self-check (answer in your notes without looking)
- What does PUBLIC on `target_include_directories` do for consumers?
- What is the `app` target and who creates it?
- Name the 4 broad stages `west build` runs.

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/02_hello_lib` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 02`), `PROGRESS.md` updated (`/progress`), committed
