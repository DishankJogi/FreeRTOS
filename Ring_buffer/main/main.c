// In this program exachange the data between tasks using ring buffer. Accordingly LED will Blink
// Task Sending Data to the buffer it will glow RED LED
// Task Receiving Data from the buffer it will glow Green LED
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

#define BLINK_GPIO 48
#define CONFIG_BLINK_LED_RMT_CHANNEL 0

static led_strip_t *pStrip_a;

TaskHandle_t task_send_Handle = NULL;
TaskHandle_t task_receive_Handle = NULL;
RingbufHandle_t ringbuffer;

//Configuration Of LED
static void configure_led(void)
{
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}
// Red LED On Function Transmitting Data
void turn_on_led(void)
{
    pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);// Here we can change the color of led
    pStrip_a->refresh(pStrip_a, 0);
}
// Green LED on Receiving Data
void turn_on_led1(void)
{
    pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);// Here we can change the color of led
    pStrip_a->refresh(pStrip_a, 0);
}

// LED Off Function
void turn_off_led(void)
{
    pStrip_a->clear(pStrip_a, 0);
    pStrip_a->refresh(pStrip_a, 0);
}

// Send data to the buffer Task
void task_send(void *arg)
{
   static char tx_item[] = "Dishank Jogi";
   xRingbufferSend(ringbuffer, tx_item, sizeof(tx_item), pdMS_TO_TICKS(100));
   printf("Send item: %s \n", tx_item);
   turn_on_led();//Red Led means Data is Transmitting
   while (1)
   {
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

// Receive data from the buffer Task
void task_receive(void *arg)
{
   size_t item_size;
   char *item = (char *)xRingbufferReceive(ringbuffer, &item_size, pdMS_TO_TICKS(100));

   vRingbufferReturnItem(ringbuffer, (void *)item);
   printf("Received item: %s \n", item);
   printf("Received item size: %d \n", item_size);
   turn_on_led1();//Green Led means Data is Received
   while (1)
   {
      vTaskDelay(pdMS_TO_TICKS(1000));
      turn_off_led();//Turn off the LED
   }
}

void app_main()
{
	//Configures the LED
	configure_led();
   // RingBuffer creation
   ringbuffer = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);

   // Write data to RingBuffer
   xTaskCreate(task_send, "task_send", 4096, NULL, 10, &task_send_Handle);
   vTaskDelay(pdMS_TO_TICKS(1000));

   // Read data from RingBuffer
   xTaskCreate(task_receive, "task_receive", 4096, NULL, 10, &task_receive_Handle);
   vTaskDelay(pdMS_TO_TICKS(1000));
}
