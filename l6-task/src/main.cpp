#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct device *my_led_sensor = DEVICE_DT_GET(DT_NODELABEL(my_led_sensor0));

int main(void)
{
    if (!device_is_ready(my_led_sensor)) {
        LOG_ERR("our_driver not ready");
        return -ENODEV;
    }

    struct sensor_value value;

    while (1) {
        sensor_sample_fetch(my_led_sensor);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        sensor_channel_get(my_led_sensor, SENSOR_CHAN_ALL, &value);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
