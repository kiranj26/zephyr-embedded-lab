#!/usr/bin/env bash
# build.sh <app> [--menuconfig | extra -D cmake args...]
set -euo pipefail
source "$(dirname "$0")/lib.sh"
resolve_app "${1:-}"; shift || true
ensure_container
ENV='source /opt/venv/bin/activate && source /opt/toolchains/zephyr/zephyr-env.sh >/dev/null'
TTY=""; [ -t 1 ] && TTY="-t"

if [ "${1:-}" = "--menuconfig" ]; then
  [ -d "$BUILD_HOST" ] || die "Build $APP_NAME first."
  exec docker exec -it "$CONTAINER" bash -c "$ENV && west build -d $BUILD_CTR -t menuconfig"
fi

# Plain CMake (non-Zephyr) apps, e.g. course/02_demo_cmake: build + run on the container's CPU
if ! grep -q 'find_package(Zephyr' "$SRC_HOST/CMakeLists.txt"; then
  info "$APP_NAME is plain CMake → building & running natively in the container"
  exec docker exec $TTY "$CONTAINER" bash -c \
    "cmake -S $SRC_CTR -B $BUILD_CTR -G Ninja && cmake --build $BUILD_CTR && echo '--- run ---' && find $BUILD_CTR -maxdepth 1 -type f -perm -u+x -exec {} \;"
fi

ARGS=""
for ext in overlay conf; do
  f="$SRC_HOST/boards/$BOARD_NAME.$ext"
  if [ -f "$f" ]; then
    [ $ext = overlay ] && ARGS="$ARGS -DDTC_OVERLAY_FILE=$SRC_CTR/boards/$BOARD_NAME.overlay"
    [ $ext = conf ]    && ARGS="$ARGS -DEXTRA_CONF_FILE=$SRC_CTR/boards/$BOARD_NAME.conf"
  fi
done

# Auto-detect out-of-tree board roots (lesson 12)
if [ -z "${BOARD_ROOT:-}" ] && [ "$BOARD_NAME" != esp32s3_devkitc ]; then
  if ls -d "$ROOT"/workspace/boards/*/"$BOARD_NAME" >/dev/null 2>&1; then BOARD_ROOT=/workspace
  elif ls -d "$ROOT"/course/workspace/boards/*/"$BOARD_NAME" >/dev/null 2>&1; then BOARD_ROOT=/course; fi
fi
[ -n "${BOARD_ROOT:-}" ] && ARGS="$ARGS -DBOARD_ROOT=$BOARD_ROOT"
ARGS="$ARGS $*"

CMD="west build -p ${PRISTINE:-always} -b $BOARD -s $SRC_CTR -d $BUILD_CTR --$ARGS"
info "[$APP_KIND] $APP_NAME"
echo "  $CMD"
docker exec $TTY "$CONTAINER" bash -c "$ENV && $CMD"
info "Output: ${BUILD_HOST#$ROOT/}/zephyr/zephyr.bin"
