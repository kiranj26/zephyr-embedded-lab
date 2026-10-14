# Lesson 09 — Interrupts and Workqueues

**Video:** Part 9 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Interrupts and Workqueues](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-9-interrupts-and-workqueues) ·
**Course code:** `09_demo_gpio_interrupt`, `09_demo_timer`, `09_demo_counter_interrupt`, `09_solution_workqueue` ·
**Your starter:** `workspace/apps/09_debounce` ·
**Hardware:** button GPIO5→GND

> Interrupts give fast reactions; workqueues move the slow part out of ISR context. Debouncing is the classic example.

## 1. Before you watch (5 min)
- Run `/lesson 09` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 09` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] GPIO interrupt setup + callback
- [ ] ISR restrictions
- [ ] `k_timer` (kernel timer) vs the hardware counter (timer0)
- [ ] Workqueues and `k_work_delayable` for deferral/debounce

## 3. Run the reference (read-only)
```sh
make run APP=course/09_demo_gpio_interrupt
make run APP=course/09_demo_timer
make run APP=course/09_demo_counter_interrupt
make run APP=course/09_solution_workqueue
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/09_debounce`
1. Count raw interrupts per press in the starter (see the bounce)
2. Add `k_work_delayable` + reschedule-from-ISR debounce
3. Print raw vs debounced counts
4. Write in notes: when to use ISR / k_timer / k_work / thread

## 5. Challenge (from the video)
Use a workqueue to run work on button press and debounce it. Course solution: `09_solution_workqueue`.

Stuck? `/hint 09 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 09`.

## 6. Go deeper (pick at least two)
- [ ] Stopwatch: button start/stop + counter alarm
- [ ] Measure ISR latency with a GPIO toggle + logic analyzer
- [ ] Use your own workqueue (`k_work_queue_start`) instead of the system one

## 7. Self-check (answer in your notes without looking)
- Why does `k_work_reschedule` debounce while `k_work_schedule` wouldn't?
- Which context does a `k_timer` expiry function run in?
- Why must you re-arm the counter alarm in its callback?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/09_debounce` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 09`), `PROGRESS.md` updated (`/progress`), committed
