// To Store the Value At Particular Location Of RAM
#include <stdio.h>
#include "esp_system.h"

void app_main() {

	// Get the value From the technical reference manual of esp32s3-wroom-1
    uintptr_t sram_start = 0x3FFBE000;
    uintptr_t sram_end = 0x3FFDFFFF;

    //Free Size SRAM
    size_t sram_size = esp_get_free_heap_size();
    printf("Size of the SRAM: %u bytes\n", sram_size);

    printf("Address range is Between 0x%08X - 0x%08X\n",sram_start,sram_end);

    //Check the value before the storing
    uint8_t* sramAddr = (uint8_t*) 0x3FFFD010;
    printf("Before Storing Value at address 0x3FFFD010 is: %u\n",*sramAddr);

    //Store the Value
    *sramAddr = 42;

    // Check the Value After the storing
    uint8_t* sramAddr2 = (uint8_t*) 0x3FFFD010;
    printf("After Storing Value at address 0x3FFFD010 is: %u\n",*sramAddr2);
}
