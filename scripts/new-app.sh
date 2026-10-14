#!/usr/bin/env bash
# new-app.sh <name>  — copy workspace/apps/_template to workspace/apps/<name>
set -euo pipefail
source "$(dirname "$0")/lib.sh"
N="${1:?usage: make new-app NAME=13_my_idea}"
D="$ROOT/workspace/apps/$N"; [ -e "$D" ] && die "$D exists"
cp -R "$ROOT/workspace/apps/_template" "$D"
sed -i '' "s/app_template/$N/" "$D/CMakeLists.txt" 2>/dev/null || sed -i "s/app_template/$N/" "$D/CMakeLists.txt"
info "Created workspace/apps/$N → make run APP=$N"
