//In this Program When Button is Pressed Queue is overwrite with new Message which will send through ISR
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

#define ESP_INR_FLAG_DEFAULT 0
#define CONFIG_BUTTON_PIN 0

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
QueueHandle_t queue1;

TaskHandle_t ISR = NULL;

void IRAM_ATTR button_isr_handler(void *arg)
{
	char txbuff[100];

	sprintf(txbuff,"Pressed the key");
	xQueueSendFromISR(queue1,&txbuff,NULL);
}

void task1(void *arg)
{
	char txbuff[100];
	queue1 = xQueueCreate(1,sizeof(txbuff));

	if(queue1 == 0)
	{
		printf("Failed to creating queue");
	}
	sprintf(txbuff,"Embedded System");
	if(xQueueOverwrite(queue1,(void *)txbuff)!=1)
	{
		printf("Could not send this message\n");
	}
	while(1)
	{
		printf("Data Waiting to read %d available space %d \n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task2(void *arg)
{
char rxbuff[100];

while(1)
{
	if(xQueueReceive(queue1,&(rxbuff),(TickType_t)5))
	{
		printf("Data Recieved From Buffer is : %s \n",rxbuff);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
}

void app_main(void)
{
	gpio_pad_select_gpio(CONFIG_BUTTON_PIN);
	gpio_set_direction(CONFIG_BUTTON_PIN,GPIO_MODE_INPUT);
	gpio_set_intr_type(CONFIG_BUTTON_PIN,GPIO_INTR_NEGEDGE);
	gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);
    gpio_isr_handler_add(CONFIG_BUTTON_PIN,button_isr_handler,NULL);

	xTaskCreate(task1,"task1",4096,NULL,10,&myTask1Handle);
	xTaskCreatePinnedToCore(task2,"task2",4096,NULL,10,&myTask2Handle,1);

}
