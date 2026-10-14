# 03 · Learning plan

**Pace:** about 5–6 h/week means 1 lesson/week, so ~13 weeks plus the capstone. With more time, do 2/week, but never skip step 4–6 below.

## The session loop (90–120 min)
1. **Warm-up (5 min):** `/lesson NN`. Answer the 2 warm-up questions from memory.
2. **Watch (20–40 min):** tick the "watch guide" boxes in the lesson README. Pause and predict before each build.
3. **Run reference (10 min):** `make run APP=course/NN_demo...`. Read the code; ask `/explain` on anything unclear.
4. **Rebuild (30–45 min):** in `workspace/apps/NN_*` from the TODOs. No copy-paste.
5. **Challenge (20–40 min):** own attempt → `/hint` ladder → only then the solution → `/review`.
6. **Consolidate (10 min):** `/notes NN`, `/quiz NN` (or every other session), `/progress`, `git commit`.

## Schedule
| Week | Lesson(s) | Milestone |
|---|---|---|
| 0 | Setup | `make doctor` green, blink runs |
| 1 | 01 Getting started | SOS blink in your own words |
| 2 | 02 CMake | your own library linked |
| 3 | 03 Kconfig | your own Kconfig module |
| 4 | 04 Devicetree | overlay from memory |
| 5 | 05 Bindings | knob-controlled PWM |
| 6–7 | 06 Drivers + workshop | **your own MCP9808 driver** (the big one, so give it two weeks) |
| 8 | 07 Debugging | breakpoint + memory examine; USB console |
| 9 | 08 Threads | producer/consumer with queue, right-sized stacks |
| 10 | 09 Interrupts | debounced button, raw vs debounced counts |
| 11 | 10 LVGL | live temperature on the screen |
| 12 | 11 Wi-Fi | HTTP GET with the HTTP client, reconnect logic |
| 13 | 12 Custom board | `s3_n16r8` board with display + sensor, no overlays |
| 14–15 | Capstone | below |

## Capstone: "desk weather node"
On your `s3_n16r8` board:
- Sensor thread (MCP9808, your driver), then a queue, then a UI thread (LVGL: temperature + trend line)
- Button (interrupt + workqueue debounce) cycles screens. The knob sets an alarm threshold, and the LED warns when it's crossed.
- Wi-Fi: POST or GET to a web endpoint every minute, with reconnect + backoff
- Kconfig options for features (`CONFIG_APP_WIFI`, `CONFIG_APP_UI`), logging throughout, README with a wiring diagram
- Stretch: OTA with MCUboot, power measurements, or BLE

## After the course
Zephyr docs (Application Development, Devicetree HOWTOs, Board Porting Guide), Zephyr samples in `/opt/toolchains/zephyr/samples`,
the Zephyr Discord, and Shawn's device-driver workshop repo if you skipped it.
