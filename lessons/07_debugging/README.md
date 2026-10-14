# Lesson 07 — Debugging with OpenOCD and GDB

**Video:** Part 7 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Debugging with OpenOCD and GDB](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-7-debugging-with-openocd-and-gdb) ·
**Course code:** `01_blink (debug target)`, `07_solution_usb_print` ·
**Your starter:** `workspace/apps/07_usb_console` ·
**Hardware:** USB cable on the board's **USB** port; LED GPIO13

> Breakpoints and memory inspection beat printf for hard bugs. The DevKitC has a built-in JTAG debugger. No extra hardware needed.

## 1. Before you watch (5 min)
- Run `/lesson 07` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 07` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] UART port (USB-UART bridge) vs USB port (built-in USB-JTAG)
- [ ] OpenOCD on the host ↔ GDB in the container via `host.docker.internal:3333`
- [ ] GDB basics: break, continue, next/step/finish, info locals, x (examine memory)
- [ ] VS Code launch.json debugging

## 3. Run the reference (read-only)
```sh
brew install open-ocd   # once
make flash APP=01  (via UART), then move cable to USB port
make openocd   # leave running
make shell → run GDB (see .claude/skills/openocd-gdb-debug)
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/07_usb_console`
1. Debug blink: break in main, watch `state`, examine GPIO_OUT `x /1x 0x60004004` before/after the set
2. Build with `-DCONFIG_DEBUG_OPTIMIZATIONS=y` and notice breakpoints behave better
3. Challenge: move the console to the USB port

## 5. Challenge (from the video)
Get printk/printf output through the USB port instead of UART. Hint: the `chosen` node in `esp32s3_devkitc_procpu.dts`. Course solution: `07_solution_usb_print` (the blog's link points at the wrong folder).

Stuck? `/hint 07 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 07`.

## 6. Go deeper (pick at least two)
- [ ] Decode a crash: copy PC/backtrace from `make capture` and run addr2line
- [ ] Set a watchpoint on a variable
- [ ] Try the VS Code debugger UI

## 7. Self-check (answer in your notes without looking)
- Why can't you step-debug through the UART port?
- Why does this lab not publish port 3333 from Docker?
- What does `mon reset halt` do vs `continue`?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/07_usb_console` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 07`), `PROGRESS.md` updated (`/progress`), committed
