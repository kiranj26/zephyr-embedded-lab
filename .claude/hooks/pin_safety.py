#!/usr/bin/env python3
"""PostToolUse hook: after a devicetree file is edited, run the pin checker.
On problems, exit 2 so Claude sees the message and fixes the pin choice."""
import json, os, subprocess, sys

try:
    data = json.load(sys.stdin)
except Exception:
    sys.exit(0)
path = (data.get("tool_input", {}) or {}).get("file_path", "")
if not path.endswith((".overlay", ".dts", ".dtsi")):
    sys.exit(0)
root = os.environ.get("CLAUDE_PROJECT_DIR", os.getcwd())
r = subprocess.run([sys.executable, os.path.join(root, "scripts", "check_pins.py"), path],
                   capture_output=True, text=True)
if r.returncode != 0:
    print("Pin safety check failed for ESP32-S3-DevKitC-1 (N16R8):\n" + r.stdout, file=sys.stderr)
    sys.exit(2)
sys.exit(0)
