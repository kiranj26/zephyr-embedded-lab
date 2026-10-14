#!/usr/bin/env bash
# capture.sh [seconds] — reset board, print serial output for N seconds, exit. Safe for automation.
set -euo pipefail
source "$(dirname "$0")/lib.sh"
P=$(detect_port)
py "$ROOT/scripts/capture.py" --port "$P" --baud "$MONITOR_BAUD" --seconds "${1:-8}" ${NORESET:+--no-reset}
