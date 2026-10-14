# Lesson 12 — How to Create a Custom Board Definition

**Video:** Part 12 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [How to Create a Custom Board Definition](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-12-how-to-create-a-custom-board-definition) ·
**Course code:** `12_demo_custom_board (+boards/custom/nyan)`, `12_solution_custom_board (+boards/custom/my_board)` ·
**Your starter:** `workspace/apps/12_board_blink` ·
**Hardware:** LED GPIO13 (+ display/sensor for the challenge)

> Real products use custom PCBs. A board definition makes apps portable and overlay-free.

## 1. Before you watch (5 min)
- Run `/lesson 12` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 12` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] HWMv2 files: board.yml, Kconfig.<board>, <board>_procpu.dts, pinctrl dtsi, defconfig, board.cmake
- [ ] Out-of-tree boards via BOARD_ROOT
- [ ] Why the app has no overlay anymore

## 3. Run the reference (read-only)
```sh
BOARD=nyan/esp32s3/procpu make run APP=course/12_demo_custom_board
BOARD=my_board/esp32s3/procpu make run APP=course/12_solution_custom_board   # see known issue #4
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/12_board_blink`
1. Read every file in `workspace/boards/custom/s3_n16r8/` and annotate it in notes
2. `BOARD=s3_n16r8/esp32s3/procpu make run APP=12`
3. Check which module dtsi/SoC symbols exist for N16R8 in this Zephyr revision (see Kconfig.s3_n16r8 comments)
4. Add the display + MCP9808 to your board; run your lesson-10 UI with zero overlays

## 5. Challenge (from the video)
Create a board that includes the display and run the LVGL demo on it. Course solution: `boards/custom/my_board` + `12_solution_custom_board`.

Stuck? `/hint 12 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 12`.

## 6. Go deeper (pick at least two)
- [ ] Add a `s3_n16r8_procpu.yaml` (twister metadata) and research what it's for
- [ ] Enable PSRAM if your Zephyr revision supports it for octal modules
- [ ] Capstone (docs/03-learning-plan.md)

## 7. Self-check (answer in your notes without looking)
- How does `s3_n16r8/esp32s3/procpu` become `BOARD_S3_N16R8_ESP32S3_PROCPU`?
- What's the bug in the course's `Kconfig.my_board`?
- When should something go in the board definition vs an app overlay?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/12_board_blink` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 12`), `PROGRESS.md` updated (`/progress`), committed
