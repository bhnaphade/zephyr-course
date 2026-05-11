#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "my_led_sensor.h"

#define DT_DRV_COMPAT my_led_sensor

LOG_MODULE_REGISTER(my_led_sensor, LOG_LEVEL_INF);

/* Dynamic data struct */
struct my_led_sensor_data {
    uint32_t blink_count;
};

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static int led_sensor_sample_fetch(const struct device *dev,
				     enum sensor_channel chan) {
    struct my_led_sensor_data *data = dev->data;

    gpio_pin_set_dt(&led, 1);
    data->blink_count++;
    LOG_INF("LED State: ON (blink_count=%u)", data->blink_count);
    return 0;
}

static int led_sensor_channel_get(const struct device *dev, 
                                enum sensor_channel chan,
                                struct sensor_value *val) {

    gpio_pin_set_dt(&led, 0);
    LOG_INF("LED State: OFF");
    return 0;
}

int my_led_sensor_set_blink_count(const struct device *dev, uint32_t count)
{
    struct my_led_sensor_data *data = dev->data;

    data->blink_count = count;
    LOG_INF("blink_count set to %u", count);
    return 0;
}

static int init(const struct device *dev) {
    if (!gpio_is_ready_dt(&led)) {
        return -ENODEV;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return -ENODEV;
    LOG_INF("Sensor LED Driver Initialized");

    return 0;
}

static const struct sensor_driver_api api_my_led_sensor = {
    .channel_get = led_sensor_channel_get,
    .sample_fetch = led_sensor_sample_fetch,
};

static struct my_led_sensor_data my_led_sensor_data_0;

DEVICE_DT_INST_DEFINE(0, init, NULL, &my_led_sensor_data_0, NULL, POST_KERNEL, 80, &api_my_led_sensor);
