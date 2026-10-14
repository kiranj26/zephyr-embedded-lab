#!/usr/bin/env bash
# Flash from the Mac (Docker on macOS cannot access USB).
set -euo pipefail
source "$(dirname "$0")/lib.sh"
resolve_app "${1:-}"
BIN="$BUILD_HOST/zephyr/zephyr.bin"
[ -f "$BIN" ] || die "No binary at ${BIN#$ROOT/}. Run: make build APP=${1}"
P=$(detect_port)
info "Flashing $APP_NAME → $P @ $BAUD"
py -m esptool --port "$P" --chip esp32s3 --baud "$BAUD" --before default_reset --after hard_reset \
  write_flash -u --flash_mode keep --flash_freq 40m --flash_size detect 0x0 "$BIN" || {
  echo; echo "Flash failed. Try one of:"
  echo "  • hold BOOT, tap RESET, release BOOT, then: make flash APP=$1"
  echo "  • slower: BAUD=460800 make flash APP=$1"
  echo "  • another cable / the other USB-C port (see docs/01-hardware.md)"; exit 1; }
