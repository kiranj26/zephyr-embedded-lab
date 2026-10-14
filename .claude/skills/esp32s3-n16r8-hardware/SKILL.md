---
name: esp32s3-n16r8-hardware
description: Pin map, forbidden pins, USB ports, wiring per lesson, and electrical rules for the ESP32-S3-DevKitC-1 with the WROOM-1-N16R8 module. Use before suggesting any GPIO, wiring, or new part, and when serial ports or flashing misbehave.
---
# ESP32-S3-DevKitC-1 (N16R8) hardware facts

Module: ESP32-S3-WROOM-1-N16R8, which has 16 MB quad-SPI flash and 8 MB **octal** PSRAM. Logic level 3.3 V.
Shawn's course board was the N32R8V variant (also octal PSRAM), so the course pins also work on this board.

## Never use
| GPIO | Reason |
|---|---|
| 26–32 | SPI flash |
| 33–37 | Octal PSRAM (R8 modules) |
| 19, 20 | Native USB D-/D+ (lesson 7 uses this port) |
| 43, 44 | UART0 TX/RX, the console through the UART bridge |

## Use with care
0 (BOOT button, strapping), 3, 45, 46 (strapping). RGB LED (WS2812): GPIO48 on v1.0, GPIO38 on v1.1.
`&gpio1 N` in devicetree means GPIO(32+N).

## Course pin plan (all safe)
| Lesson | Signal | GPIO |
|---|---|---|
| 1,2,4,7,8 | LED (via 330 Ω to GND) | 13 |
| 4,9 | Button → GND (internal pull-up, active low) | 5 |
| 6 | Second button | 4 |
| 5 | Potentiometer wiper (ADC1_CH0) | 1 |
| 5 | PWM LED (LEDC ch0) | 13 |
| 6,8 | MCP9808 SDA / SCL (I2C0) | 15 / 16 |
| 10,12 | ST7735R 160×80: SCK 12, MOSI 10, CS 9, DC 18, RST 8 (MISO 11 unused) | — |

## Ports
- "UART" USB-C → CP210x bridge → `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`. Used for flashing and the console.
- "USB" USB-C → native USB-Serial/JTAG → `/dev/cu.usbmodem*`. Used for the lesson 7 console and the OpenOCD debugger.
- Manual bootloader: hold BOOT, tap RST, release BOOT.

Validate overlays with `python3 scripts/check_pins.py`. Full details: `docs/01-hardware.md`.
