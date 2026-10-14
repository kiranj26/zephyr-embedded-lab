#!/usr/bin/env python3
"""Pin-safety checker for ESP32-S3-DevKitC-1 (N16R8) devicetree files.

Usage: python3 scripts/check_pins.py <file.overlay|.dts|.dtsi> [...]
Exit 0 = OK, 1 = problems found. Used by the Claude Code PostToolUse hook too.
"""
import re, sys

FORBIDDEN = {
    **{p: "connected to SPI flash" for p in range(26, 33)},
    **{p: "used by the OCTAL PSRAM on N16R8/R8 modules" for p in range(33, 38)},
    19: "native USB D-", 20: "native USB D+",
    43: "UART0 TX (your console)", 44: "UART0 RX (your console)",
}
WARN = {
    0: "BOOT strapping pin (fine as a button input, don't drive it)",
    3: "strapping pin (JTAG source select)", 45: "strapping pin (VDD_SPI voltage)",
    46: "strapping pin (boot mode / ROM log)",
    48: "onboard WS2812 RGB LED on DevKitC-1 v1.0 (not a plain LED)",
    38: "onboard WS2812 RGB LED on DevKitC-1 v1.1 (not a plain LED)",
}
ALLOWED_SIGNALS = {("UART0_TX", 43), ("UART0_RX", 44)}

def pins_in(text):
    for m in re.finditer(r"&gpio([01])\s+(\d+)", text):
        yield int(m.group(2)) + (32 if m.group(1) == "1" else 0), m.group(0), None
    for m in re.finditer(r"\b([A-Z0-9_]+?)_GPIO(\d+)\b", text):
        yield int(m.group(2)), m.group(0), m.group(1)

def check(path):
    try:
        text = open(path, encoding="utf-8").read()
    except OSError as e:
        return [f"{path}: cannot read ({e})"], []
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    errs, warns = [], []
    for pin, src, sig in pins_in(text):
        if sig and (sig, pin) in ALLOWED_SIGNALS:
            continue
        if pin in FORBIDDEN:
            errs.append(f"{path}: GPIO{pin} ({src}) — {FORBIDDEN[pin]}. Pick another pin (see docs/01-hardware.md).")
        elif pin in WARN:
            warns.append(f"{path}: GPIO{pin} ({src}) — {WARN[pin]}.")
        elif pin > 48:
            errs.append(f"{path}: GPIO{pin} does not exist on ESP32-S3.")
    return errs, warns

def main(paths):
    all_e, all_w = [], []
    for p in paths:
        e, w = check(p); all_e += e; all_w += w
    for w in all_w: print("⚠ " + w)
    for e in all_e: print("✖ " + e)
    if not all_e and paths: print("✔ pins OK: " + ", ".join(paths))
    return 1 if all_e else 0

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(0)
    sys.exit(main(sys.argv[1:]))
