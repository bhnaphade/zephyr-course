#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "my_led_sensor.h"

LOG_MODULE_REGISTER(sensor_shell, LOG_LEVEL_INF);

#define BLINK_COUNT_MAX 10U

static const struct device *sensor_dev = DEVICE_DT_GET(DT_NODELABEL(my_led_sensor0));

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    int ret = sensor_sample_fetch(sensor_dev);

    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }
    shell_print(sh, "sensor_sample_fetch called successfully");
    return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    struct sensor_value val;
    int ret = sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &val);

    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }
    shell_print(sh, "sensor_channel_get: result: %d val: %d", ret, val.val1);
    return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "Device name : %s", sensor_dev->name);
    shell_print(sh, "Device ready: %s", device_is_ready(sensor_dev) ? "yes" : "no");
    return 0;
}

static int cmd_sensor_set_blink_count(const struct shell *sh, size_t argc, char **argv)
{
    char *endptr;
    long val = strtol(argv[1], &endptr, 10);

    if (*endptr != '\0') {
        shell_error(sh, "Invalid argument '%s': must be a number", argv[1]);
        return -EINVAL;
    }
    if (val < 0 || val > BLINK_COUNT_MAX) {
        shell_error(sh, "Value %ld out of range [0, %u]", val, BLINK_COUNT_MAX);
        return -EINVAL;
    }

    int ret = my_led_sensor_set_blink_count(sensor_dev, (uint32_t)val);

    if (ret < 0) {
        shell_error(sh, "set_blink_count failed: %d", ret);
        return ret;
    }
    shell_print(sh, "blink_count set to %ld", val);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmds,
    SHELL_CMD_ARG(fetch, NULL, "Call sensor_sample_fetch", cmd_sensor_fetch, 1, 0),
    SHELL_CMD_ARG(read,  NULL, "Call sensor_channel_get and print value", cmd_sensor_read, 1, 0),
    SHELL_CMD_ARG(info,  NULL, "Print device name and ready state", cmd_sensor_info, 1, 0),
    SHELL_CMD_ARG(set_blink_count, NULL, "Set blink count <0-10>", cmd_sensor_set_blink_count, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmds, "LED sensor commands", NULL);
