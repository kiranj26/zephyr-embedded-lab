#!/usr/bin/env python3
"""PreToolUse hook: block edits to the read-only course/ submodule.
Exit code 2 blocks the tool call and shows stderr to Claude."""
import json, os, sys

try:
    data = json.load(sys.stdin)
except Exception:
    sys.exit(0)
inp = data.get("tool_input", {}) or {}
path = inp.get("file_path") or inp.get("notebook_path") or ""
root = os.environ.get("CLAUDE_PROJECT_DIR", os.getcwd())
course = os.path.realpath(os.path.join(root, "course")) + os.sep
if path and os.path.realpath(os.path.join(root, path)).startswith(course):
    print("course/ is the read-only reference submodule. Copy the app into workspace/apps/ "
          "(e.g. `cp -R course/workspace/apps/X workspace/apps/X_mine`) and edit the copy.", file=sys.stderr)
    sys.exit(2)
sys.exit(0)
