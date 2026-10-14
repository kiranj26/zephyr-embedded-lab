#!/usr/bin/env python3
"""Reset the ESP32-S3 via the UART bridge's RTS line, then print serial output for N seconds.

DevKitC-1 auto-reset wiring: RTS -> EN (reset), DTR -> GPIO0 (boot mode).
Holding DTR low/false keeps GPIO0 high, so the chip boots normally from flash.
"""
import argparse, sys, time
import serial

ap = argparse.ArgumentParser()
ap.add_argument("--port", required=True)
ap.add_argument("--baud", type=int, default=115200)
ap.add_argument("--seconds", type=float, default=8)
ap.add_argument("--no-reset", action="store_true")
a = ap.parse_args()

with serial.Serial(a.port, a.baud, timeout=0.1) as s:
    if not a.no_reset and "usbmodem" not in a.port:
        s.dtr = False; s.rts = True; time.sleep(0.1); s.rts = False
    print(f"--- capturing {a.port} for {a.seconds:g}s ---", flush=True)
    end = time.time() + a.seconds
    while time.time() < end:
        data = s.read(1024)
        if data:
            sys.stdout.write(data.decode("utf-8", errors="replace")); sys.stdout.flush()
    print("\n--- end capture ---")
