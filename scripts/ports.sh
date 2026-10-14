#!/usr/bin/env bash
echo "Serial ports (macOS: always use /dev/cu.*):"
ls /dev/cu.* 2>/dev/null | grep -viE 'bluetooth|debug-console|wlan' | sed 's/^/  /' || echo "  (none — check cable/driver)"
cat <<'TXT'

ESP32-S3-DevKitC-1 has two USB-C ports:
  "UART" port → cu.usbserial-* / cu.SLAB_USBtoUART  → flashing + console (default for every lesson)
  "USB"  port → cu.usbmodem*                         → native USB: lesson 7 (USB console, JTAG debug)
TXT
