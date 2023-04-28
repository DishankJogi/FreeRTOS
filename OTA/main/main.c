#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
//#include "esp_event_loop.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "sdkconfig.h"
#include "esp_netif.h"

#define WIFI_SSID "SIQOL"
#define WIFI_PASS "Siqol@20221215"


#define OTA_TASK_PRIORITY  1
#define OTA_TASK_STACKSIZE  8192

// Event group
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// The URL of the firmware update binary file on Google Drive
#define OTA_URL "https://drive.google.com/uc?export=download&id=1rrOD940OQ7EtE3Sf8ekRmgY2xWx2tY3T"

// The OTA task handle
TaskHandle_t ota_task_handle = NULL;

// receive buffer
int rcv_buffer[200];

// esp_http_client event handler
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {

	switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;
        case HTTP_EVENT_HEADER_SENT:
            break;
        case HTTP_EVENT_ON_HEADER:
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                memcpy(rcv_buffer, evt->data, evt->data_len);
            }
            // handle the binary data received
            break;
        case HTTP_EVENT_ON_FINISH:
            break;
        case HTTP_EVENT_DISCONNECTED:
            break;
    }
    return ESP_OK;
}
esp_partition_t *ota_0_partition;
void ota_task(void *pvParameter)
{
	while(1)
	{
		ESP_LOGI("OTA Task", "Starting OTA update from %s", OTA_URL);
		esp_http_client_config_t config =
		{
				.url = OTA_URL,
				.event_handler = _http_event_handler,
		};

		//Downloading the firmware
		esp_http_client_handle_t client = esp_http_client_init(&config);
		esp_err_t err = esp_http_client_perform(client);
		esp_err_t ret = esp_https_ota(&config);
		if (ret == ESP_OK)
		{
			ESP_LOGI("OTA Task", "OTA update success");
			esp_restart();
		}
		else
		{
			ESP_LOGE("OTA Task", "OTA update failed");
		}
		// cleanup
		esp_http_client_cleanup(client);
		vTaskDelay(30000 / portTICK_PERIOD_MS);
	}
}


// Wifi event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {

    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;

	case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        xTaskCreate(&ota_task, "ota_task", OTA_TASK_STACKSIZE, NULL, OTA_TASK_PRIORITY, &ota_task_handle);
        break;

	case SYSTEM_EVENT_STA_DISCONNECTED:
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;

	default:
        break;
    }

	return ESP_OK;
}


// Main task
void main_task(void *pvParameter)
{
	// wait for connection
	printf("Main task: waiting for connection to the wifi network...\n");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
	printf("connected!\n");

	// print the local IP address
	tcpip_adapter_ip_info_t ip_info;
	ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	printf("IP Address:  %s\n", ip4addr_ntoa(&ip_info.ip));
	printf("Subnet mask: %s\n", ip4addr_ntoa(&ip_info.netmask));
	printf("Gateway:     %s\n", ip4addr_ntoa(&ip_info.gw));
	while(1)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}


// Main application
void app_main()
{
	// disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);

	// initialize NVS
	ESP_ERROR_CHECK(nvs_flash_init());

	// create the event group to handle wifi events
	wifi_event_group = xEventGroupCreate();

	// initialize the tcp stack
	tcpip_adapter_init();

	// initialize the wifi event handler
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	// initialize the wifi stack in STAtion mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// configure the wifi connection and start the interface
	wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
	printf("Connecting to %s\n", WIFI_SSID);

	// start the main task
    xTaskCreate(&main_task, "main_task", 2048, NULL, 5, NULL);

    // Delete the OTA task handle
        if (ota_task_handle != NULL)
        {
            vTaskDelete(ota_task_handle);
        }
}
