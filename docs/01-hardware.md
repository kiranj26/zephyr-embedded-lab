# 01 · Hardware: ESP32-S3-DevKitC-1 (ESP32-S3-WROOM-1-N16R8)

| Spec | Value |
|---|---|
| Module | ESP32-S3-WROOM-1-**N16R8**: 16 MB quad flash, 8 MB **octal** PSRAM |
| Logic | 3.3 V. **Never put 5 V on a GPIO.** |
| USB-C "UART" | USB-to-UART bridge → `/dev/cu.usbserial-*` / `cu.SLAB_USBtoUART` → flashing + console (UART0 on GPIO43/44) |
| USB-C "USB" | native USB-Serial/JTAG → `/dev/cu.usbmodem*` → lesson 7 console + OpenOCD |
| Buttons | BOOT (GPIO0), RST (EN) |
| RGB LED | WS2812 on GPIO48 (board v1.0) or GPIO38 (v1.1). Addressable, **not** a plain LED. |

Shawn's course board is the N32R8V variant, which also uses octal PSRAM. The course's pin choices are therefore safe on your board too.
The target `esp32s3_devkitc/esp32s3/procpu` describes an N8 module. Your extra flash and PSRAM are simply unused. That's fine.

## Pin rules (enforced by `scripts/check_pins.py` and a Claude Code hook)
| GPIO | Status | Why |
|---|---|---|
| 26–32 | ✖ never | SPI flash |
| 33–37 | ✖ never | octal PSRAM (R8) |
| 19, 20 | ✖ avoid | USB D-/D+ |
| 43, 44 | ✖ avoid | UART0 console |
| 0, 3, 45, 46 | ⚠ care | strapping pins (GPIO0 = BOOT; fine as input) |
| 38 / 48 | ⚠ | onboard RGB LED |
| 1–18, 21, 38–42, 47 | ✔ | general use (check ADC needs: ADC1 = GPIO1–10) |

## Wiring by lesson
| Lesson | Part | Connections |
|---|---|---|
| 1, 2, 4, 7, 8 | LED | GPIO13 → 330 Ω → LED anode; cathode → GND |
| 4, 9 | Button | GPIO5 → button → GND (internal pull-up) |
| 6 (demo) | Buttons ×2 | GPIO4 → btn → GND; GPIO5 → btn → GND |
| 5 | Potentiometer | ends → 3V3, GND; wiper → GPIO1 (ADC1_CH0). LED stays on GPIO13 (PWM) |
| 6, 8 | MCP9808 | VDD → 3V3, GND → GND, SDA → GPIO15, SCL → GPIO16, A0–A2 → GND (addr 0x18) |
| 7 | — | second USB-C cable on the USB port (debug / USB console) |
| 10, 12 | ST7735R 160×80 | VCC → 3V3, GND, SCK → GPIO12, MOSI(SDA) → GPIO10, CS → GPIO9, DC → GPIO18, RST → GPIO8, BLK → 3V3 |
| 11 | — | 2.4 GHz WPA2 Wi-Fi network |

## Parts list (what the course uses)
- Adafruit **1782** MCP9808 breakout (or any MCP9808 board)
- Adafruit **3533** 0.96" 160×80 TFT (ST7735R). The init values in the overlays assume this panel type.
- LEDs, 220–330 Ω resistors, 2 tactile buttons, 10 kΩ potentiometer, breadboard(s), jumpers, 2 USB-C data cables
- Optional: multimeter, 8-channel logic analyzer + PulseView

Tip: the DevKitC-1 is wide. Use two breadboards side by side so both header rows are reachable.
