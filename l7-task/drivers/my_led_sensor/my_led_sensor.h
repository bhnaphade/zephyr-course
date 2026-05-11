#ifndef MY_LED_SENSOR_H
#define MY_LED_SENSOR_H

#include <zephyr/device.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Extension API: update the blink count stored in the driver's data structure */
int my_led_sensor_set_blink_count(const struct device *dev, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* MY_LED_SENSOR_H */
