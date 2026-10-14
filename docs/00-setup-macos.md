# 00 · Setup on macOS (once, ~1 hour mostly waiting)

```
 Mac (host)                                     Docker container "zephyr-lab"
 ├─ venv/: esptool + pyserial ─ USB ─► board     ├─ /opt/toolchains/zephyr  (pinned commit)
 ├─ workspace/  ───────── bind mount ──────────► ├─ /workspace     (your code, read-write)
 ├─ course/workspace ──── bind mount (ro) ─────► ├─ /course        (reference, read-only)
 └─ OpenOCD (lesson 7) ◄─ host.docker.internal ─ └─ GDB, west, code-server :8800
```
Docker Desktop on macOS can't pass USB devices into containers. So **build in the container, flash and monitor on the Mac**.

## Steps
1. **Prerequisites:** Docker Desktop (running; Settings → Resources: ≥ 8 GB RAM, ≥ 40 GB disk),
   Xcode CLT (`xcode-select --install`), Homebrew (for Python 3.12 if missing).
2. **Get the repo and the course:**
   ```sh
   git clone --recursive https://github.com/kiranj26/zephyr-embedded-lab.git && cd zephyr-embedded-lab
   make bootstrap        # adds github.com/ShawnHymel/introduction-to-zephyr as submodule course/
   ```
3. **Host tools:** `make setup` creates `venv/` with `esptool==4.8.1` and `pyserial==3.5`, the versions the course uses.
4. **Image:** `make image` builds `course/Dockerfile.espressif` for your CPU (arm64 on Apple Silicon).
   - If it fails downloading the SDK/host tools on arm64: `PLATFORM=linux/amd64 make image` (emulated; enable Rosetta in Docker Desktop).
   - Flaky VS Code extension downloads: just re-run.
5. **Start:** `make up`. Open http://localhost:8800, or in VS Code use *Dev Containers: Attach to Running Container* → `zephyr-lab` →
   *File → Open Workspace from File* → `/zephyr.code-workspace`. To browse the reference in the editor, *Add Folder to Workspace* → `/course`.
6. **Board:** plug the **UART** USB-C port. `make ports` should show `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`.
   If nothing appears: try a data cable; if still nothing, install Silicon Labs' CP210x VCP driver for macOS.
7. **Check:** `make doctor`, then `make run APP=01`. Quit the monitor with **Ctrl+]**.

## Claude Code
Install Claude Code, run `claude` in the repo root. It loads `CLAUDE.md` and everything in `.claude/`.
See `docs/06-claude-code.md`.

## If flashing fails
Hold **BOOT**, tap **RST**, release **BOOT**, then retry. Or try `BAUD=460800 make flash APP=01`.
