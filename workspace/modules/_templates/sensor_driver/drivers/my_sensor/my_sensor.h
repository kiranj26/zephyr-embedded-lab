#ifndef MY_SENSOR_H_
#define MY_SENSOR_H_

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

/* TODO: register map from the datasheet (MCP9808: TA=0x05, RESOLUTION=0x08, MANUF_ID=0x06, DEVICE_ID=0x07) */
#define MY_SENSOR_REG_TEMP        0x05
#define MY_SENSOR_REG_RESOLUTION  0x08

struct my_sensor_config {          /* const, filled from devicetree at build time */
	struct i2c_dt_spec i2c;
	uint8_t resolution;
};

struct my_sensor_data {            /* runtime state */
	uint16_t raw;
};

#endif /* MY_SENSOR_H_ */
