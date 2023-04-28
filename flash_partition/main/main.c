// Read and Write Integer Data onto flash memory
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

void app_main(void)
{
    // Initialize the NVS partition
    nvs_flash_init();

    // Wait for memory initialization
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Define memory handle
    nvs_handle_t my_handle;

    // Open storage with a given namespace
    nvs_open("storage", NVS_READWRITE, &my_handle);

    //For the Read data from the flash
    int32_t read_data;
    nvs_get_i32(my_handle, "data", &read_data);
    printf("Read data: %d \n", read_data);

    //For the Write data onto the flash
    int32_t write_data = 2222;
    nvs_set_i32(my_handle, "data", write_data);
    printf("Write data: %d \n", write_data);

    nvs_commit(my_handle);
    nvs_close(my_handle);
}




