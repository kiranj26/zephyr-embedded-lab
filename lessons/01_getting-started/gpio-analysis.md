# GPIO analysis: how to check a pin before you use it

A "pin profile" answers one question: **is this pin free and suitable for what I want, now and in later lessons?**
Build it from real sources (the chip, the module, the board, the devicetree), not from memory or a random pinout image.

---

## The method: 7 checks

| # | Question | Where to look |
|---|---|---|
| 1 | **Is it even usable on my module?** (flash, PSRAM, USB, console) | `docs/01-hardware.md` pin rules; `scripts/check_pins.py` |
| 2 | **Is it a strapping pin?** (read at reset, decides the boot mode) | ESP32-S3 datasheet, "Strapping Pins": GPIO0, 3, 45, 46 |
| 3 | **Which GPIO controller and cell?** | `zephyr.dts`: `gpio0` = 0–31, `gpio1` = 32+ (cell = GPIO − 32) |
| 4 | **What special functions does it have?** (IO MUX, ADC, touch, RTC) | Espressif HAL headers: `io_mux_reg.h`, `adc_channel.h`, `rtc_io_channel.h`, `touch_sensor_channel.h` |
| 5 | **Can peripherals be routed to it?** (GPIO matrix) | `zephyr/dt-bindings/pinctrl/esp32s3-pinctrl.h`: macros named `<SIGNAL>_GPIO<n>` |
| 6 | **Does anything in *my* devicetree already claim it?** | `build/.../zephyr/zephyr.dts`: search for the pin in `gpios`, and in `pinmux` of **enabled** nodes |
| 7 | **Will a later lesson need it?** | `docs/01-hardware.md` "Wiring by lesson" |

Then a quick **electrical** sanity check: 3.3 V logic only, never 5 V. Use a series resistor for LEDs: (3.3 V − ~2 V LED) / 330 Ω ≈ 4 mA.

Useful commands (in the container: `make shell`):
```sh
P=/opt/toolchains/modules/hal/espressif/components/soc/esp32s3/include/soc
grep -n "_GPIO17 \|U1TXD" $P/io_mux_reg.h           # IO MUX functions
grep -n "GPIO17" $P/adc_channel.h $P/rtc_io_channel.h  # ADC / RTC
grep -c "_GPIO17 " /opt/toolchains/zephyr/include/zephyr/dt-bindings/pinctrl/esp32s3-pinctrl.h
```
And on the Mac:
```sh
grep -n "17" workspace/build/01_blink/zephyr/zephyr.dts
python3 scripts/check_pins.py workspace/apps/01_blink/boards/esp32s3_devkitc.overlay
```

---

## Worked example: GPIO17 (2026-10-14)

| # | Check | Finding | Source |
|---|---|---|---|
| 1 | Usable on N16R8? | ✔ Not flash/PSRAM (26–37), USB (19/20) or console (43/44) | `check_pins.py` → "pins OK" |
| 2 | Strapping? | ✔ No | datasheet (0, 3, 45, 46 only) |
| 3 | Controller / cell | `gpio0`, cell **17** (17 < 32) | `zephyr.dts`: `gpio0` has `ngpios = <0x20>` (32) |
| 4a | IO MUX | Function 1 = **GPIO17**, function 2 = **U1TXD** (UART1 TX). The register is named `DAC_1` (a leftover name from the original ESP32) | `io_mux_reg.h:289-290` |
| 4b | ADC | **ADC2 channel 6**. Note: ADC2 can't be used while Wi-Fi is on (lesson 11) | `adc_channel.h:57` |
| 4c | RTC | **RTC GPIO 17**, so it can wake the chip from deep sleep | `rtc_io_channel.h:61` |
| 4d | Touch | ✖ No. Touch pads are GPIO1–14 | `touch_sensor_channel.h` |
| 5 | GPIO matrix | **165** routable signals (I2C, I2S, SPI, UART…). Almost any peripheral can use it | `esp32s3-pinctrl.h` |
| 6 | Claimed in my DT? | The board's `i2s0_default` pinctrl uses `I2S0_I_SD_GPIO17`, **but `i2s0` is `status = "disabled"`**, so it isn't applied. `uart1` (which could use 17) is disabled too. ✔ Free | board `esp32s3_devkitc-pinctrl.dtsi:54`; `zephyr.dts:253-265` |
| 7 | Later lessons? | ✔ Not used by any lesson (18 is the display's DC in lessons 10 and 12) | `docs/01-hardware.md` |

**Verdict:** ✔ GPIO17 is a good permanent second-LED pin. Keep in mind: if you ever enable `i2s0` with the board's
default pinctrl, or route UART1 TX here, GPIO17 stops being free.

### Things this example teaches
- **A pin has many possible jobs, but only one at a time.** The IO MUX and GPIO matrix decide which one. Plain GPIO is just one option.
- **A pinctrl group only matters when its device is enabled.** The board file "mentions" GPIO17 for I2S, and that's harmless while `i2s0` is disabled. Always check `status` in the merged `zephyr.dts`, not just whether the pin appears.
- **ADC2 and Wi-Fi conflict.** For analog input later, prefer ADC1 pins (GPIO1–10). That's why the course's pot uses GPIO1.

---

## Your turn
Do the same 7 checks for **GPIO13** (your LED 1 pin) and fill in this table. Hint for check 4: GPIO13 *is* a touch pin.

| # | Check | Finding | Source |
|---|---|---|---|
| 1 | Usable? | | |
| 2 | Strapping? | | |
| 3 | Controller / cell | | |
| 4 | IO MUX / ADC / RTC / touch | | |
| 5 | GPIO matrix count | | |
| 6 | Claimed in my DT? | | |
| 7 | Later lessons? | | |
