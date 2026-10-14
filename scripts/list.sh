#!/usr/bin/env bash
source "$(dirname "$0")/lib.sh"
echo "Your apps (workspace/apps) → make run APP=<prefix>"
ls -1 "$ROOT/workspace/apps" 2>/dev/null | grep -v '^_' | sed 's/^/  /'
echo; echo "Course reference apps (read-only) → make run APP=course/<prefix>"
ls -1 "$ROOT/course/workspace/apps" 2>/dev/null | grep -v README | sed 's/^/  course\//' || echo "  (run make bootstrap)"
echo; echo "Custom boards: $(ls "$ROOT/workspace/boards/custom" 2>/dev/null | tr '\n' ' ')(yours)  $(ls "$ROOT/course/workspace/boards/custom" 2>/dev/null | tr '\n' ' ')(course)"
