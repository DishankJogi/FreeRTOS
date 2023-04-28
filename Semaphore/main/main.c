// In this Example Send the data to from the task 1 to task two using Semaphores
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore = NULL;

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

void task1(void *arg)
{
	while(1)
	{
		printf("Sent Message: [%d]\n",xTaskGetTickCount());
		xSemaphoreGive(xSemaphore);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
	}
void task2(void *arg)
{
	while(1)
	{
		if(xSemaphoreTake(xSemaphore,portMAX_DELAY))
		{
			printf("Got Message: [%d]\n",xTaskGetTickCount());
		}
	}
}
void app_main(void)
{
	xSemaphore = xSemaphoreCreateBinary();
	xTaskCreate(task1,"task1",4096,NULL,10,&myTask1Handle);
	xTaskCreatePinnedToCore(task2,"task2",4096,NULL,10,&myTask2Handle,1);
}
