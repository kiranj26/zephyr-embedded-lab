# Lesson 06 — Device Driver Development

**Video:** Part 6 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [Device Driver Development](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-6-device-driver-development) ·
**Course code:** `06_demo_device_driver`, `06_solution_i2c_driver`, `(modules/button, modules/mcp9808)` ·
**Your starter:** `workspace/apps/06_my_driver` ·
**Hardware:** 2 buttons GPIO4/5; MCP9808 SDA15/SCL16

> This is where Zephyr 'clicks': devicetree + Kconfig + CMake + C come together into a reusable driver.

## 1. Before you watch (5 min)
- Run `/lesson 06` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 06` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] Module layout: `zephyr/module.yaml`, Kconfig, CMake, `dts/bindings`
- [ ] `DT_DRV_COMPAT`, config vs data structs, init priority
- [ ] `DEVICE_DT_INST_DEFINE` + `DT_INST_FOREACH_STATUS_OKAY`
- [ ] A custom API struct (button) vs the standard sensor API (MCP9808)

## 3. Run the reference (read-only)
```sh
make run APP=course/06_demo_device_driver
make run APP=course/06_solution_i2c_driver
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/06_my_driver`
1. Trace one call from the app to `gpio_pin_get_dt` inside `course/workspace/modules/button`
2. Copy `modules/_templates/sensor_driver` → `modules/mcp9808_mine`; rename compatible, symbols, files
3. Write the overlay (pinctrl + i2c0 + node + alias) and finish the driver TODOs
4. `make run APP=06`, then `/review 06`

## 5. Challenge (from the video)
Work through github.com/ShawnHymel/workshop-zephyr-device-driver: an MCP9808 I2C sensor driver. Course solution: `06_solution_i2c_driver` + `modules/mcp9808`.

Stuck? `/hint 06 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 06`.

## 6. Go deeper (pick at least two)
- [ ] Read the MCP9808 manufacturer/device ID in init and fail if wrong
- [ ] Compare with Zephyr's in-tree driver (`grep -rl mcp9808 /opt/toolchains/zephyr/drivers`)
- [ ] Capture the I2C transaction with a logic analyzer

## 7. Self-check (answer in your notes without looking)
- Config struct vs data struct: which is const and why?
- What does `__device_dts_ord_N undefined` usually mean?
- How does Zephyr know to compile your driver only when a node exists?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/06_my_driver` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 06`), `PROGRESS.md` updated (`/progress`), committed
