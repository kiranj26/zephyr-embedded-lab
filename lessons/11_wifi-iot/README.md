# Lesson 11 — WiFi and IoT

**Video:** Part 11 in the [Introduction to Zephyr playlist](https://www.youtube.com/playlist?list=PLEBQazB0HUyTmK2zdwhaf8bLwuEaDH-52) ·
**Blog / written version:** [WiFi and IoT](https://www.digikey.com/en/maker/tutorials/2025/introduction-to-zephyr-part-11-wifi-and-iot) ·
**Course code:** `11_demo_wifi`, `11_solution_http` ·
**Your starter:** `workspace/apps/11_http_client` ·
**Hardware:** 2.4 GHz WPA2 Wi-Fi network

> Connectivity is why many people pick the ESP32. Zephyr's net_mgmt + sockets APIs are portable across chips.

## 1. Before you watch (5 min)
- Run `/lesson 11` in Claude Code, or skim this page.
- Hardware ready? Run `/wire 11` and double-check the pins in `docs/01-hardware.md`.

## 2. Watch guide — look for these
- [ ] Wi-Fi management API and events
- [ ] Semaphores to wait for async connect/DHCP
- [ ] DNS + BSD sockets + manual HTTP GET
- [ ] Why `wifi.c` is split from `main.c` (CMake globbing src/*.c)

## 3. Run the reference (read-only)
```sh
cp the SSID/PSK into a COPY of the course app first (course/ is read-only): cp -R course/workspace/apps/11_demo_wifi workspace/apps/11_course_wifi
```
Then read the code. Treat `course/` as a textbook. Don't edit it.

## 4. Build it yourself in `workspace/apps/11_http_client`
1. `cp src/secrets.h.example src/secrets.h` and fill it in
2. Write `wifi.c` yourself from the TODO spec
3. DNS → socket → send → recv loop
4. Print total bytes and close everything

## 5. Challenge (from the video)
Use Zephyr's HTTP client subsystem (`CONFIG_HTTP_CLIENT`, `http_client_req`) instead of hand-built requests. Course solution: `11_solution_http`.

Stuck? `/hint 11 1`, then level 2, then 3. Open the solution only after a real attempt, then `/review 11`.

## 6. Go deeper (pick at least two)
- [ ] Reconnect with backoff on disconnect
- [ ] Fetch and parse a small JSON API (`include/zephyr/data/json.h`)
- [ ] Enable the net shell and inspect interfaces

## 7. Self-check (answer in your notes without looking)
- Why semaphores and not busy-waiting for Wi-Fi events?
- What does `zsock_getaddrinfo` return and what must you free?
- Why will a 5 GHz-only network never work here?

## Done when
- [ ] Reference runs on my board
- [ ] My version in `workspace/apps/11_http_client` runs and I can explain every line
- [ ] Challenge attempted before peeking, then reviewed
- [ ] Two "go deeper" items done
- [ ] `notes.md` written (`/notes 11`), `PROGRESS.md` updated (`/progress`), committed
