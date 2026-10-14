# 05 · Known issues and traps (course repo + macOS)

1. **README path typo.** The course README says `apps/01_demo_blink`. The folder is `apps/01_blink`.
2. **Two Zephyr versions mentioned.** The Docker image uses the pinned commit in `Dockerfile.espressif`. `workspace/west.yml`
   (v4.0.0) is not used by the course flow. Don't run `west init/update` in `/workspace`, and don't upgrade mid-course
   (the LVGL code is v8 API; newer Zephyr ships LVGL 9).
3. **Port 3333 vs OpenOCD.** The course's `docker run -p 3333:3333` makes Docker hold host port 3333, so OpenOCD on the Mac can't bind it.
   GDB reaches the host via `host.docker.internal:3333` anyway, so the lab's `make up` doesn't publish it. `make doctor` warns if something does.
4. **`boards/custom/my_board/Kconfig.my_board`** says `select SOC_ESP32S3_PROCPU if BOARD_NYAN_ESP32S3_PROCPU`, copy-pasted from nyan.
   For my_board the condition should be `BOARD_MY_BOARD_ESP32S3_PROCPU`. If the course's lesson-12 solution fails on SoC/Kconfig errors,
   this is the first suspect. (You can't edit `course/`. Copy the board into `workspace/boards/custom/` to experiment.)
5. **`my_board_procpu_defconfig`** sets `CONFIG_MAIN_STACK_SIZE` twice (2048, then 4096). The last value wins.
6. **`10_demo_display` overlay** repeats `reg = <0>;`, and its `madctl` comment disagrees with the solution's comment.
7. **Hard-coded Wi-Fi credentials** in course lesson 11. Use `secrets.h` (git-ignored) in your apps.
8. **`11_solution_http/src/wifi.c`** defines `void wifi_disconnect(void)`, which is not in its header (the demo returns `int`).
9. **Root `.vscode/settings.json`** in the course repo points at a Windows path. Harmless; ignore it.
10. **Part 7 blog** links the wrong solution folder (`08_solution_sensor_queue` instead of `07_solution_usb_print`).
11. **macOS bash is 3.2.** The scripts avoid bash-4 features. Use `/dev/cu.*`, not `/dev/tty.*`. There's no `dialout` group.
12. **Apple Silicon image builds** are arm64. If the SDK download fails, use `PLATFORM=linux/amd64 make image`.
13. **Styles on the stack (lesson 10 course code)** only work because `main` never returns. Make LVGL styles `static`.
