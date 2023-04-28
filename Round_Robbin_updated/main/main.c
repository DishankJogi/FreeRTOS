//Program For Round Robbin Scheduling
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "led_strip.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

static const char *TAG = "example";

#define TASK_1_INTERVAL_MS 500
#define TASK_2_INTERVAL_MS 1000
#define TIME_QUANTUM_MS 100

#define BLINK_GPIO 48
#define BUTTON_GPIO 0
#define CONFIG_BLINK_LED_RMT_CHANNEL 0

static led_strip_t *pStrip_a;

TaskHandle_t task1_handle, task2_handle;
int current_task = 0;

//Configuration Of LED
static void configure_led(void)
{
    //ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}
// Blue LED On Function
void turn_on_led(void)
{
    pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);// Here we can change the color of led
    pStrip_a->refresh(pStrip_a, 0);
}
// Red LED On Function
void turn_on_led1(void)
{
    pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);// Here we can change the color of led
    pStrip_a->refresh(pStrip_a, 0);
}
// LED Off Function
void turn_off_led(void)
{
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}

// Button Configuration
void configure_button(void)
{
	gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);
}


void task1(void *pvParameters)
{
    while (1) {
        // Task 1 logic
        printf("Task 1 running...\n");
        turn_on_led();
        vTaskDelay(TASK_1_INTERVAL_MS / portTICK_PERIOD_MS);
        turn_off_led();
    }
}

void task2(void *pvParameters)
{
    while (1) {
        // Task 2 logic
        printf("Task 2 running...\n");
        turn_on_led1();
        vTaskDelay(TASK_2_INTERVAL_MS / portTICK_PERIOD_MS);
        turn_off_led();
    }
}

void round_robin_scheduler(void *pvParameters)
{
    while (1) {
        vTaskDelay(TIME_QUANTUM_MS / portTICK_PERIOD_MS);

        // Determine which task to run next
        if (current_task == 0) {
            current_task = 1;
            vTaskResume(task2_handle);
            vTaskSuspend(task1_handle);
        } else {
            current_task = 0;
            vTaskResume(task1_handle);
            vTaskSuspend(task2_handle);
        }
    }
}

void app_main()
{

	configure_led();
	configure_button();
    // Create task 1
    xTaskCreate(task1, "Task 1", 4096, NULL, 1, &task1_handle);

    // Create task 2
    xTaskCreate(task2, "Task 2", 4096, NULL, 1, &task2_handle);

    // Create round-robin scheduler task
    xTaskCreate(round_robin_scheduler, "Scheduler", 4096, NULL, 2, NULL);
}
