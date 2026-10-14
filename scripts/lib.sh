#!/usr/bin/env bash
# Shared config + helpers. Compatible with macOS bash 3.2.
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
: "${BOARD:=esp32s3_devkitc/esp32s3/procpu}"   # official board target for ESP32-S3-DevKitC-1
: "${IMAGE:=env-zephyr-espressif}"            # Docker image built from course/Dockerfile.espressif
: "${CONTAINER:=zephyr-lab}"
: "${BAUD:=921600}"
: "${MONITOR_BAUD:=115200}"
BOARD_NAME="${BOARD%%/*}"                        # e.g. esp32s3_devkitc, s3_n16r8, nyan

die()  { echo "✖ $*" >&2; exit 1; }
info() { echo "▸ $*"; }

py() { [ -x "$ROOT/venv/bin/python" ] || die "Host venv missing. Run: make setup"; "$ROOT/venv/bin/python" "$@"; }

# resolve_app <spec>  -> sets APP_NAME, APP_KIND (mine|course), SRC_HOST, SRC_CTR, BUILD_HOST, BUILD_CTR
#   "04"               -> workspace/apps/04_*
#   "course/04_demo"   -> course/workspace/apps/04_demo*  (read-only reference)
resolve_app() {
  local spec="$1" base kind prefix matches n
  [ -n "$spec" ] || die "APP is required (e.g. APP=01 or APP=course/01_blink). See: make list"
  case "$spec" in
    course/*) kind=course; spec="${spec#course/}"; base="$ROOT/course/workspace/apps" ;;
    *)        kind=mine;   base="$ROOT/workspace/apps" ;;
  esac
  [ -d "$base" ] || die "$base not found (did you run make bootstrap?)"
  if [ -d "$base/$spec" ]; then matches="$spec"
  else matches=$(cd "$base" && ls -d ${spec}*/ 2>/dev/null | sed 's#/$##' || true); fi
  [ -n "$matches" ] || die "No app matches '$spec' in ${base#$ROOT/}. See: make list"
  n=$(echo "$matches" | wc -l | tr -d ' ')
  if [ "$n" -gt 1 ]; then echo "Ambiguous '$spec':" >&2; echo "$matches" | sed 's/^/   /' >&2; die "Be more specific."; fi
  APP_NAME="$matches"; APP_KIND="$kind"
  if [ "$kind" = course ]; then
    SRC_HOST="$base/$APP_NAME"; SRC_CTR="/course/apps/$APP_NAME"; prefix="course-"
  else
    SRC_HOST="$base/$APP_NAME"; SRC_CTR="/workspace/apps/$APP_NAME"; prefix=""
  fi
  BUILD_HOST="$ROOT/workspace/build/$prefix$APP_NAME"
  BUILD_CTR="/workspace/build/$prefix$APP_NAME"
}

ensure_container() {
  docker ps --format '{{.Names}}' 2>/dev/null | grep -qx "$CONTAINER" || die "Container '$CONTAINER' not running. Run: make up"
}

detect_port() {
  if [ -n "${PORT:-}" ]; then echo "$PORT"; return; fi
  local p
  # Prefer the UART bridge port (CP210x / CH34x) over native USB
  p=$(ls /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART* /dev/cu.wchusbserial* 2>/dev/null | head -1 || true)
  [ -z "$p" ] && p=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true)
  [ -n "$p" ] || die "No ESP32 serial port found. Plug the UART port in, then: make ports"
  echo "$p"
}
