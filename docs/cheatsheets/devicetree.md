# Cheatsheet · Devicetree
| Syntax | Meaning |
|---|---|
| `label: name@unit { ... };` | node with a label (for `&label` references) |
| `&label { ... };` | modify an existing node (overlay) |
| `aliases { my-led = &led0; };` | portable name → `DT_ALIAS(my_led)` |
| `chosen { zephyr,console = &uart0; };` | system-level choices → `DT_CHOSEN(zephyr_console)` |
| `compatible = "vendor,part";` | selects the binding and driver (`DT_DRV_COMPAT vendor_part`) |
| `status = "okay";` | enable the node (drivers instantiate only okay nodes) |
| `gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>;` | phandle + specifier cells |
| `pinctrl-0 = <&grp>; pinctrl-names = "default";` | pin mux for a peripheral |

C macros: `DT_ALIAS`, `DT_NODELABEL`, `DT_CHOSEN`, `DT_PROP`, `DEVICE_DT_GET`, `GPIO_DT_SPEC_GET`, `PWM_DT_SPEC_GET`,
`ADC_CHANNEL_CFG_DT`, `I2C_DT_SPEC_INST_GET`, `DT_INST_FOREACH_STATUS_OKAY`. Remember dashes become underscores in C names.
