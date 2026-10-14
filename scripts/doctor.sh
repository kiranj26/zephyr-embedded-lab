#!/usr/bin/env bash
# Checks every link in the chain and says what to fix.
source "$(dirname "$0")/lib.sh"
ok(){ echo "  ✔ $*"; }; bad(){ echo "  ✖ $*"; FAIL=1; }; FAIL=0
echo "Zephyr Embedded Lab doctor"
[ -f "$ROOT/course/README.md" ] && ok "course/ submodule present" || bad "course/ missing → make bootstrap"
[ -x "$ROOT/venv/bin/python" ] && ok "host venv" || bad "host venv missing → make setup"
"$ROOT/venv/bin/python" -c "import serial, esptool" 2>/dev/null && ok "pyserial + esptool" || bad "pip packages → make setup"
command -v docker >/dev/null && ok "docker CLI" || bad "docker not installed"
docker info >/dev/null 2>&1 && ok "docker daemon running" || bad "start Docker Desktop"
docker image inspect "$IMAGE" >/dev/null 2>&1 && ok "image $IMAGE" || bad "image missing → make image"
docker ps --format '{{.Names}}' 2>/dev/null | grep -qx "$CONTAINER" && ok "container $CONTAINER running" || bad "container not running → make up"
if docker ps --format '{{.Ports}}' 2>/dev/null | grep -q '0.0.0.0:3333'; then bad "some container publishes :3333 — this blocks OpenOCD on the Mac"; fi
P=$(ls /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART* /dev/cu.wchusbserial* /dev/cu.usbmodem* 2>/dev/null | head -1)
[ -n "$P" ] && ok "board port $P" || bad "no board port → plug UART port, try another cable, make ports"
command -v openocd >/dev/null && ok "openocd (optional, lesson 7)" || echo "  · openocd not installed (only needed for lesson 7: brew install open-ocd)"
[ $FAIL = 0 ] && echo "All good. Try: make run APP=01" || exit 1
