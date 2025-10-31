#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "wifi_sniffer.h"
#include "usb_gadget.h"

static const char *TAG = "main";

#define WIFI_CHANNEL_DEFAULT 1
#define CHANNEL_SWITCH_INTERVAL_MS 5000

static uint8_t current_channel = WIFI_CHANNEL_DEFAULT;
static uint32_t packet_count = 0;

/**
 * @brief WiFi packet callback - sends packet to USB
 */
static void wifi_packet_handler(const uint8_t *buf, uint32_t len)
{
    packet_count++;

    // Only send if USB is connected
    if (usb_gadget_is_connected()) {
        // Create a simple packet header with metadata
        char header[64];
        int header_len = snprintf(header, sizeof(header), 
                                  "\n[CH:%d PKT:%lu LEN:%lu] ", 
                                  wifi_sniffer_get_channel(), 
                                  packet_count, 
                                  len);

        // Send header
        usb_gadget_send((uint8_t *)header, header_len);

        // Send packet data (first 64 bytes for demonstration)
        uint32_t send_len = (len > 64) ? 64 : len;
        usb_gadget_send(buf, send_len);
    }

    // Log periodically
    if (packet_count % 100 == 0) {
        ESP_LOGI(TAG, "Captured %lu packets on channel %d", 
                 packet_count, wifi_sniffer_get_channel());
    }
}

/**
 * @brief Task to switch WiFi channels periodically
 */
static void channel_switch_task(void *pvParameters)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_SWITCH_INTERVAL_MS));
        
        current_channel++;
        if (current_channel > 13) {
            current_channel = 1;
        }

        wifi_sniffer_set_channel(current_channel);
        ESP_LOGI(TAG, "Switched to channel %d", current_channel);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "=== WiFi Sniffer with USB Gadget ===");
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());

    // Initialize USB gadget first
    ESP_LOGI(TAG, "Initializing USB gadget...");
    esp_err_t ret = usb_gadget_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize USB gadget: %s", esp_err_to_name(ret));
        return;
    }

    // Wait a bit for USB to stabilize
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Initialize WiFi sniffer
    ESP_LOGI(TAG, "Initializing WiFi sniffer...");
    ret = wifi_sniffer_init(WIFI_CHANNEL_DEFAULT, wifi_packet_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi sniffer: %s", esp_err_to_name(ret));
        return;
    }

    // Start sniffing
    ESP_LOGI(TAG, "Starting WiFi sniffing...");
    ret = wifi_sniffer_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi sniffer: %s", esp_err_to_name(ret));
        return;
    }

    // Create channel switching task
    xTaskCreate(channel_switch_task, "channel_switch", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "WiFi Sniffer started successfully!");
    ESP_LOGI(TAG, "Connect via USB to see captured packets");
    ESP_LOGI(TAG, "Channels will cycle every %d ms", CHANNEL_SWITCH_INTERVAL_MS);

    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        ESP_LOGI(TAG, "Status: Packets=%lu, Channel=%d, USB=%s",
                 packet_count, 
                 wifi_sniffer_get_channel(),
                 usb_gadget_is_connected() ? "Connected" : "Disconnected");
    }
}
