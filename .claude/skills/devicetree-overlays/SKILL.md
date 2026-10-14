---
name: devicetree-overlays
description: Patterns and debugging steps for Zephyr devicetree overlays on ESP32-S3 - aliases, chosen, gpio-leds, gpio-keys, ADC channels, LEDC PWM, pinctrl groups, I2C/SPI devices, MIPI-DBI displays, and custom bindings. Use when writing, reading, or fixing .overlay/.dts files.
---
# Devicetree overlay cookbook (ESP32-S3, pinned 2024 Zephyr)

## LED + button (lessons 1, 4)
```dts
/ {
    aliases { my-led = &led_0; my-button = &button_0; };
    leds    { compatible = "gpio-leds";  led_0: d13 { gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>; }; };
    buttons { compatible = "gpio-keys";  button_0: d5 { gpios = <&gpio0 5 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>; }; };
};
```
C side: `GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios)` (dashes become underscores in C).

## ADC channel (lesson 5)
`&adc0 { status="okay"; #address-cells=<1>; #size-cells=<0>; adc0_ch0: channel@0 { reg=<0>; zephyr,gain="ADC_GAIN_1_4"; zephyr,reference="ADC_REF_INTERNAL"; zephyr,vref-mv=<3900>; zephyr,acquisition-time=<ADC_ACQ_TIME_DEFAULT>; zephyr,resolution=<12>; }; };`
ADC1 channel 0 is GPIO1. C side: `ADC_CHANNEL_CFG_DT(node)`, `DT_PROP(node, zephyr_vref_mv)`.

## PWM LED via LEDC (lesson 5)
Requires `#include <zephyr/dt-bindings/pwm/pwm.h>`, a `pwm-leds` node with `pwms = <&ledc0 0 1000 PWM_POLARITY_NORMAL>`
(the period is in ns in that cell), `&ledc0 { pinctrl-0=<&ledctrl_0>; pinctrl-names="default"; status="okay"; channel0@0 { reg=<0>; timer=<0>; }; }`,
and a pinctrl group `pinmux = <LEDC_CH0_GPIO13>; output-enable;`.

## I2C sensor (lesson 6)
pinctrl group `pinmux = <I2C0_SDA_GPIO15>, <I2C0_SCL_GPIO16>; bias-pull-up; drive-open-drain; output-high;`, then
`&i2c0 { pinctrl-0=<&i2c0_custom_pins>; status="okay"; mcp9808@18 { compatible="microchip,mcp9808"; reg=<0x18>; ... }; };`

## Console to native USB (lesson 7)
`/ { chosen { zephyr,console = &usb_serial; zephyr,shell-uart = &usb_serial; }; }; &usb_serial { status = "okay"; };`

## Display (lesson 10)
`#include <zephyr/dt-bindings/mipi_dbi/mipi_dbi.h>`, a `zephyr,mipi-dbi-spi` node with `spi-dev=<&spi2>`, dc/reset gpios,
and a `sitronix,st7735r` child. Set `chosen { zephyr,display = &st7735r_160x80; }`. Copy the init arrays from
`course/workspace/apps/10_demo_display/boards/esp32s3_devkitc.overlay`.

## Debugging checklist
1. Open `workspace/build/<app>/zephyr/zephyr.dts`. Is the node there, with `status = "okay"`?
2. Check the alias spelling: overlay `my-led` ↔ C `my_led`.
3. Missing macro (`GPIO_ACTIVE_LOW`, `PWM_POLARITY_NORMAL`, `MIPI_DBI_MODE_SPI_4WIRE`)? Add the right `#include`.
4. Binding errors list required properties. Read `/opt/toolchains/zephyr/dts/bindings/<class>/<compatible>.yaml`.
5. Check the overlay was actually passed (the build log shows `-DDTC_OVERLAY_FILE`; this repo passes `boards/<board>.overlay` automatically).
6. Run `python3 scripts/check_pins.py <overlay>`.
