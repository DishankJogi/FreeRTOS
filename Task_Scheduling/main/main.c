#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Task Handler for each task
TaskHandle_t mytask1Handle = NULL;
TaskHandle_t mytask2Handle = NULL;

// First Task
void task1(void *arg)
{
	int c = 0;// counter variable
	while(1)
	{
		printf("Dishank Jogi\n");
		vTaskDelay(1000/portTICK_RATE_MS);// delay of 1000 miliseconds
		c++;// Increment the counter

		// suspending the task 2 after 5 iteration
		if (c ==5)
		{
			vTaskSuspend(mytask2Handle);
			printf("Task is Suspended!!!\n");
		}
		// Resuming the task 2 after 7 iteration
		if(c==7)
		{
			vTaskResume(mytask2Handle);
			printf("Task is Resume :)\n");
		}
		// Deleting the task 2 after 10 iteration
		if(c==10)
		{
			vTaskDelete(mytask2Handle);
			printf("Task Is Deleted...\n");
		}
	}

}
//Task 2 functions

void task2(void *arg)
{
	while(1)
	{
		printf("SIQOL\n");
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

// Main Function
void app_main(void)
{
	// Creting Task 1 on to the core 0
    xTaskCreate(task1, "task1" , 4096 , NULL , 10 , &mytask1Handle);
    // Creting Task 2 on to the core 1
    xTaskCreatePinnedToCore(task2, "task2" , 4096 , NULL , 10 , &mytask2Handle,1);
}
