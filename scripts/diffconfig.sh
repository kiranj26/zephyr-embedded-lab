#!/usr/bin/env bash
# diffconfig.sh <app> — show what the last menuconfig save changed (.config.old → .config)
#   "X n -> y"  = you changed it      "+X y" = newly appeared      "-X"  = disappeared
set -euo pipefail
source "$(dirname "$0")/lib.sh"
resolve_app "${1:-}"
ensure_container
CFG="$BUILD_HOST/zephyr"
[ -f "$CFG/.config" ]     || die "No build for $APP_NAME. Run: make build APP=$1"
[ -f "$CFG/.config.old" ] || die "No .config.old yet. Change something in: make menuconfig APP=$1 (then S to save)"
info "$APP_NAME: .config.old → .config   (X n -> y = changed · +X = new · -X = gone)"
docker exec "$CONTAINER" bash -c "source /opt/venv/bin/activate && \
  python3 /opt/toolchains/zephyr/scripts/kconfig/diffconfig $BUILD_CTR/zephyr/.config.old $BUILD_CTR/zephyr/.config"
