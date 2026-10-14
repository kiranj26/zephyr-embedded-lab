#!/usr/bin/env bash
# Host-side tools: Python 3.12 venv with esptool + pyserial (versions match the course)
set -euo pipefail
source "$(dirname "$0")/lib.sh"
[ "$(uname)" = Darwin ] || echo "(not macOS — continuing anyway)"
command -v docker >/dev/null || die "Docker not found. Install Docker Desktop and start it."
docker info >/dev/null 2>&1 || die "Docker is installed but not running. Start Docker Desktop."
PY=$(command -v python3.12 || true)
if [ -z "$PY" ]; then
  command -v brew >/dev/null || die "Need Python 3.12. Install Homebrew (https://brew.sh) then re-run."
  info "Installing python@3.12 via Homebrew"; brew install python@3.12; PY=$(command -v python3.12)
fi
cd "$ROOT"
[ -d venv ] || "$PY" -m venv venv
venv/bin/python -m pip install -q --upgrade pip
venv/bin/python -m pip install -q pyserial==3.5 esptool==4.8.1
info "Host tools ready: $(venv/bin/python -m esptool version 2>/dev/null | tail -1)"
info "Next: make image"
