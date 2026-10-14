#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/lib.sh"
if [ -z "${1:-}" ]; then rm -rf "$ROOT/workspace/build" && info "Removed all builds"; exit 0; fi
resolve_app "$1"; rm -rf "$BUILD_HOST" && info "Removed ${BUILD_HOST#$ROOT/}"
