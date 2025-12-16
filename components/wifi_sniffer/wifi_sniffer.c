/**
 * @file main.c
 * @brief ESP-IDF entry point that wires together USB, Wi-Fi, and sniffer subsystems.
 */
// main.c
#include <stdio.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "wifi.h"       //wifi_mgr_init()
#include "sniffer.h"    //sniffer_init()
#include "cmd.h"        //command_init()
#include "usb_cdc.h"    //usb_cdc_init()


static const char *TAG = "WIFI_SNIF";

/**
 * @brief WIFI Sniffer Main (initialization function).
 */
void wifi_sniffer_init(void)
{
    // STEP.1 NVS Flash Initializatio
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGW(TAG, "NVS initialized");   
    
    // STEP.2 Initialize USB CDC before WiFi
    usb_cdc_init();    
    
    // STEP.3 Network Interface initialization/ESP Event Loop initialization
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // STEP.4 Initialize Wi-Fi driver in NULL mode
    wifi_init_config_t wcfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wcfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // 필요하면 기본 채널 명시적으로 설정 (예: 1ch)
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    // STEP.5 Initialize Sniffer (Ring Buffer, Streamer Task)
    sniffer_init();        
    
    // STEP.6 Enable Promiscuous Mode + Callback
    sniffer_enable_promiscuous();

    // STEP.7 Initialize Wi-Fi Manager (config cache)
    ESP_ERROR_CHECK(wifi_mgr_init());
    
    // STEP.8 Initialize Command Console
    command_init();    
    
    ESP_LOGW(TAG, "Command console initialized");
}