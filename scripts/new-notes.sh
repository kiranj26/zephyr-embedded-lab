#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/lib.sh"
N="${1:?usage: make notes N=04}"
D=$(ls -d "$ROOT"/lessons/${N}* 2>/dev/null | head -1); [ -n "$D" ] || die "No lesson $N"
F="$D/notes.md"; [ -f "$F" ] && { info "Exists: ${F#$ROOT/}"; exit 0; }
sed "s/{{LESSON}}/$(basename "$D")/; s/{{DATE}}/$(date +%Y-%m-%d)/" "$ROOT/lessons/_notes_template.md" > "$F"
info "Created ${F#$ROOT/}"
