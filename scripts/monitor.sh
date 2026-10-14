#!/usr/bin/env bash
# Interactive serial console (quit: Ctrl+]). For non-interactive use (e.g. Claude Code): make capture
set -euo pipefail
source "$(dirname "$0")/lib.sh"
P=$(detect_port)
info "Monitor $P @ $MONITOR_BAUD — quit with Ctrl+]"
py -m serial.tools.miniterm --raw "$P" "$MONITOR_BAUD"
