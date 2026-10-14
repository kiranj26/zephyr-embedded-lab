/*
 * Template I2C sensor driver (sensor API). Lesson 6 challenge scaffold.
 * Fill the TODOs using the datasheet; compare with course/workspace/modules/mcp9808 only AFTER trying.
 */
#define DT_DRV_COMPAT my_vendor_my_sensor

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include "my_sensor.h"

LOG_MODULE_REGISTER(my_sensor, CONFIG_SENSOR_LOG_LEVEL);

static int reg_read16(const struct device *dev, uint8_t reg, uint16_t *val)
{
	const struct my_sensor_config *cfg = dev->config;
	int ret = i2c_write_read_dt(&cfg->i2c, &reg, 1, val, 2);

	if (ret == 0) {
		*val = sys_be16_to_cpu(*val);
	}
	return ret;
}

static int my_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct my_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_AMBIENT_TEMP) {
		return -ENOTSUP;
	}
	return reg_read16(dev, MY_SENSOR_REG_TEMP, &data->raw);
}

static int my_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
				 struct sensor_value *val)
{
	const struct my_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_AMBIENT_TEMP) {
		return -ENOTSUP;
	}
	/* TODO: convert data->raw (13-bit two's complement, 1/16 C per LSB for MCP9808)
	 *       into val->val1 (integer C) and val->val2 (millionths). */
	ARG_UNUSED(data);
	val->val1 = 0;
	val->val2 = 0;
	return 0;
}

static int my_sensor_init(const struct device *dev)
{
	const struct my_sensor_config *cfg = dev->config;

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("I2C bus not ready");
		return -ENODEV;
	}
	/* TODO: verify the chip ID register, then write cfg->resolution to the resolution register */
	LOG_DBG("init ok (resolution %u)", cfg->resolution);
	return 0;
}

static const struct sensor_driver_api my_sensor_api = {
	.sample_fetch = my_sensor_sample_fetch,
	.channel_get = my_sensor_channel_get,
};

#define MY_SENSOR_DEFINE(inst)                                                   \
	static struct my_sensor_data my_sensor_data_##inst;                      \
	static const struct my_sensor_config my_sensor_config_##inst = {         \
		.i2c = I2C_DT_SPEC_INST_GET(inst),                               \
		.resolution = DT_INST_PROP(inst, resolution),                    \
	};                                                                       \
	SENSOR_DEVICE_DT_INST_DEFINE(inst, my_sensor_init, NULL,                 \
				     &my_sensor_data_##inst, &my_sensor_config_##inst, \
				     POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,   \
				     &my_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(MY_SENSOR_DEFINE)
