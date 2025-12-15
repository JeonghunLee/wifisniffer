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

#include "wifi_sniffer.h"       //wifi_sniffer_init()

static const char *TAG = "MAIN";


/**
 * @brief Application entry point invoked by ESP-IDF.
 */
void app_main(void)
{
    ESP_LOGW(TAG, "Wi-Fi Sniffer starting...");    
    wifi_sniffer_init();
}
