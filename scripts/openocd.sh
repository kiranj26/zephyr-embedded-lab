#!/usr/bin/env bash
# Lesson 7: run OpenOCD on the Mac against the ESP32-S3 built-in USB-JTAG ("USB" port).
set -euo pipefail
source "$(dirname "$0")/lib.sh"
command -v openocd >/dev/null || die "Install OpenOCD ≥0.12: brew install open-ocd  (or xPack OpenOCD)"
if docker ps --format '{{.Ports}}' | grep -q '0.0.0.0:3333'; then die "Port 3333 is published by a container. Stop it (make down) and use 'make up' from this repo."; fi
info "Plug the board's USB (not UART) port. GDB in the container connects to host.docker.internal:3333"
exec openocd -f board/esp32s3-builtin.cfg
