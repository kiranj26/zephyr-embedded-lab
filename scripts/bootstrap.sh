#!/usr/bin/env bash
# Adds Shawn Hymel's course repo as a read-only git submodule at course/
set -euo pipefail
source "$(dirname "$0")/lib.sh"
cd "$ROOT"
[ -d .git ] || { git init -q; info "Initialised git repo"; }
if [ ! -f course/README.md ]; then
  info "Adding course submodule (github.com/ShawnHymel/introduction-to-zephyr)"
  git submodule add https://github.com/ShawnHymel/introduction-to-zephyr.git course 2>/dev/null \
    || git submodule update --init --recursive
else
  git submodule update --init --recursive
fi
chmod +x scripts/*.sh scripts/*.py .claude/hooks/*.py 2>/dev/null || true
info "course/ ready ($(git -C course rev-parse --short HEAD)). Treat it as READ-ONLY reference."
info "Next: make setup && make image"
