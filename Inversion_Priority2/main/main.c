//Example of Priority Inversion
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
    // Red LED
    pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);
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
void HigherPriorityTask(void* pvParameters)
{
    while(1)
    {
        printf("High priority task: Trying to acquire mutex\n");
        //If semaphore is taken by task within given time it's return pdTRUE
        //Another Meaning is Semaphore Taken Successfully
        if(xSemaphoreTake(mutex, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            printf("High priority task: Acquired mutex\n");
            ets_printf("Red LED Means High Priority Task Running\n");
            // Toggle the LED
            turn_on_led();//
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            turn_off_led();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            xSemaphoreGive(mutex);
            printf("High priority task: Releasing mutex\n");
        }
        else
        {
            printf("High priority task: Unable to acquire mutex\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void LowerPriorityTask(void* pvParameters)
{
    while(1)
    {
        printf("Low priority task: Trying to acquire mutex\n");
        //If semaphore is taken by task within given time it's return pdTRUE
        //Another Meaning is Semaphore Taken Successfully
        if(xSemaphoreTake(mutex, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            printf("Low priority task: Acquired mutex\n");
            ets_printf("Green LED Means Low Priority Task Running\n");
            // Toggle the LED
            turn_on_led1();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            turn_off_led();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xSemaphoreGive(mutex);
            printf("Low priority task: Releasing mutex\n");
        }
        else
        {
            printf("Low priority task: Unable to acquire mutex\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Configure the LED
    configure_led();
    // Create the mutex
    mutex = xSemaphoreCreateMutex();

    xTaskCreate(LowerPriorityTask, "Lower Priority Task", 2048, NULL, 1, NULL);// Low Priority Task
    xTaskCreate(HigherPriorityTask, "Higher Priority Task", 2048, NULL, 2, NULL);//High Priority Task
}
