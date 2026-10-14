# Lesson 08 — Multithreading

**Video:** Part 8 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Multithreading](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-8-multithreading) ·
**Course code:** `08_demo_multithreading`, `08_demo_mutex`, `08_solution_sensor_queue` ·
**Your starter:** `workspace/apps/08_sensor_queue` ·
**Hardware:** LED GPIO13; MCP9808 (challenge)

> Threads, priorities, and safe communication are the core of any RTOS. Getting these right avoids the nastiest bugs.

## 1. Before you watch (5 min)
- Run `/lesson 08` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 08` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] Thread stacks (`K_THREAD_STACK_DEFINE`) and `k_thread_create` arguments
- [ ] Priorities: preemptive vs cooperative
- [ ] Race conditions and mutexes (console input changing blink speed)
- [ ] Message queues for producer/consumer

## 3. Run the reference (read-only)
```sh
make run APP=course/08_demo_multithreading
make run APP=course/08_demo_mutex   # type + / - then Enter in the monitor
make run APP=course/08_solution_sensor_queue
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/08_sensor_queue`
1. Implement demo (a) and (b) in the starter, then delete them
2. Producer thread (sensor) + consumer thread (printer) + `K_MSGQ_DEFINE`
3. Enable the thread analyzer and right-size stacks
4. Ask the **rtos-analyst** agent to predict your output timeline

## 5. Challenge (from the video)
Read the temperature in one thread and print batches from a queue in another. Course solution: `08_solution_sensor_queue`.

Stuck? `/hint 08 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 08`.

## 6. Go deeper (pick at least two)
- [ ] Rewrite the mutex demo with a semaphore and with `atomic_t`
- [ ] Create starvation on purpose and fix it
- [ ] What happens when the queue fills? Try `K_FOREVER` vs `K_NO_WAIT`

## 7. Self-check (answer in your notes without looking)
- Lower priority number = higher or lower priority?
- Why is `k_msgq_put(..., K_NO_WAIT)` safe in an ISR but `k_mutex_lock` isn't?
- How do you size a thread stack?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/08_sensor_queue` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 08`), `PROGRESS.md` updated (`/progress`), committed
