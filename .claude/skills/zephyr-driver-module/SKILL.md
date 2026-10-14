---
name: zephyr-driver-module
description: Step-by-step recipe for out-of-tree Zephyr modules and device drivers (module.yaml, Kconfig, CMake, DTS bindings, DT_DRV_COMPAT, DEVICE_DT_INST_DEFINE, custom API vs sensor API). Use for lesson 6, the device-driver workshop challenge, and new drivers.
---
# Out-of-tree driver module recipe

Skeleton: `workspace/modules/_templates/sensor_driver/` (copy it to `workspace/modules/<name>/` and rename `my_sensor`).
References: `course/workspace/modules/button` (custom API), `course/workspace/modules/mcp9808` (sensor API).
Challenge repo from Part 6: github.com/ShawnHymel/workshop-zephyr-device-driver

```
modules/<name>/
├── zephyr/module.yaml          name, build: {cmake: ., kconfig: Kconfig, settings: {dts_root: .}}
├── CMakeLists.txt              add_subdirectory(drivers); zephyr_include_directories(drivers)
├── Kconfig                     rsource "drivers/Kconfig"
├── dts/bindings/<class>/<vendor>,<part>.yaml   compatible + include + properties
└── drivers/
    ├── CMakeLists.txt          add_subdirectory_ifdef(CONFIG_<NAME> <name>)
    ├── Kconfig                 rsource "<name>/Kconfig"
    └── <name>/{CMakeLists.txt (zephyr_library + sources), Kconfig, <name>.c, <name>.h}
```
App side: `set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/<name>")` **before** `find_package(Zephyr)`,
then `CONFIG_<NAME>=y`, a devicetree node with the matching compatible, and `DEVICE_DT_GET(DT_ALIAS(...))`.

Driver C essentials:
- `#define DT_DRV_COMPAT vendor_part` (the compatible with `,` and `-` turned into `_`).
- `struct cfg` (const, from DT: `I2C_DT_SPEC_INST_GET(inst)`, `DT_INST_PROP(inst, prop)`) vs `struct data` (runtime).
- Init returns 0 or a negative errno. Check `device_is_ready(cfg->i2c.bus)`.
- API struct (`sensor_driver_api` or your own), then `SENSOR_DEVICE_DT_INST_DEFINE(inst, init, NULL, &data_##inst, &cfg_##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &api)`.
- `DT_INST_FOREACH_STATUS_OKAY(MY_DEFINE)` instantiates one driver per okay node.
Error "`__device_dts_ord_N` undefined" means the node exists but the driver Kconfig is off, or the compatible doesn't match.
