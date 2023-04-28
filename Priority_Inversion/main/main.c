// Priority Inversion
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include <freertos/semphr.h>

#define LED_GPIO 48
#define CONFIG_BLINK_LED_RMT_CHANNEL 0


static led_strip_t *pStrip_a;
static SemaphoreHandle_t mutex;

static const char *TAG = "example";

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, LED_GPIO, 1);
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}
void turn_on_led(void)
{
	// Blue LED
    pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
    pStrip_a->refresh(pStrip_a, 0);
}

void turn_on_led1(void)
{
	//Green LED
    pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);
    pStrip_a->refresh(pStrip_a, 0);
}

void turn_off_led(void)
{
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}
void LEDTask(void* pvParameters)
{
    while(1)
    {
    	printf("High priority task: Trying to acquire mutex\n");
        xSemaphoreTake(mutex, portMAX_DELAY);
        printf("High priority task: Acquired mutex\n");
        // Toggle the LED
        turn_on_led();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        turn_off_led();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xSemaphoreGive(mutex);
        printf("High priority task: Releasing mutex\n");
        vTaskDelete(NULL);
    }
}

void LowerPriorityTask(void* pvParameters)
{
    while(1)
    {
    	printf("Low priority task: Trying to acquire mutex\n");
    	xSemaphoreTake(mutex, portMAX_DELAY);
    	printf("Low priority task: Acquired mutex\n");
    	// Toggle the LED
    	turn_on_led1();
    	vTaskDelay(5000 / portTICK_PERIOD_MS);
        turn_off_led();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    	xSemaphoreGive(mutex);
    	printf("Low priority task: Releasing mutex\n");
        vTaskDelete(NULL);
    }
}

void app_main(void)
{
    // Configure the LED
	configure_led();

    // Create the mutex
    mutex = xSemaphoreCreateMutex();

    xTaskCreate(LowerPriorityTask, "Lower Priority Task", 2048, NULL, 1, NULL);// Low Priority Task
    xTaskCreate(LEDTask, "LED Task", 2048, NULL, 2, NULL);//High Priority Task
}
