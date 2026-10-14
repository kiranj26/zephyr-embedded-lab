# modules/

Out-of-tree Zephyr modules (libraries and drivers). Apps opt in with
`set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/<name>")` placed **before** `find_package(Zephyr)`.

- `_templates/kconfig_module/` — lesson 3: a library gated by your own Kconfig symbol
- `_templates/sensor_driver/` — lesson 6: an I2C sensor driver with a devicetree binding (sensor API)

Copy a template, then rename every `greeting` / `my_sensor` / `MY_SENSOR` / `my_vendor,my-sensor`.
