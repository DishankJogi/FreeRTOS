#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

#define BLINK_GPIO CONFIG_BLINK_GPIO
static uint8_t s_led_state = 0;

static led_strip_t *pStrip_a;

static void blink_led(void)
{
    if (s_led_state)
    {
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        pStrip_a->refresh(pStrip_a, 100);
    }
    else
    {
        pStrip_a->clear(pStrip_a, 50);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    pStrip_a->clear(pStrip_a, 50);
}

void app_main()
{
    configure_led();

    while (1) {
        ESP_LOGI(TAG, "Putting an LED AS AN ON : %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
