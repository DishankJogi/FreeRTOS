// USing AnyEdge Interrupt "Turn ON" And "Turn OFF LED"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "esp_sleep.h"

#define BLINK_GPIO 48
#define CONFIG_BLINK_LED_RMT_CHANNEL 0
#define BUTTON_GPIO 0 // Change this to the GPIO pin connected to the button

static const char *TAG = "example";

static led_strip_t *pStrip_a;

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
}

void turn_on_led(void)
{
    pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
    pStrip_a->refresh(pStrip_a, 0);
}

void turn_off_led(void)
{
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}

int button_state = 1;
int interrupt_occured = 0;

void IRAM_ATTR button_isr_handler(void *arg)
{
    button_state = gpio_get_level(BUTTON_GPIO);
    //Toggle the state
    interrupt_occured = !interrupt_occured;
    ets_printf("\t \t \t ***** Interrupt Occurred : %d ***** \t",interrupt_occured);
    //printf("Interrupt occurred");
}
void configure_button(void)
{
    gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_ANYEDGE);
}

void button_task(void *pvParameters)
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void led_task(void *pvParameters)
{
	while(1)
	{
		printf("button_state: %d\n", button_state);
		if(button_state == 0 && interrupt_occured == 1)// LED TURN ON THE RISING EDGE ( "LOW(0) To HIGH(1) TRANSITION" )
		{
			turn_on_led();
			printf("LED TURN ON\n");
		}
		else
		{
			turn_off_led();
			printf("LED TURN OFF\n");
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void app_main()
{
    configure_led();
    configure_button();
    xTaskCreate(button_task, "button_task", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(led_task, "led_task", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);
}

