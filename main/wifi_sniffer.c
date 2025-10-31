#include "wifi_sniffer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>

static const char *TAG = "wifi_sniffer";

static wifi_sniffer_packet_cb_t s_packet_callback = NULL;
static uint8_t s_current_channel = 1;

/**
 * @brief WiFi promiscuous mode callback
 */
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type)
{
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA && type != WIFI_PKT_CTRL) {
        return;
    }

    const wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buff;
    
    if (s_packet_callback != NULL) {
        s_packet_callback(pkt->payload, pkt->rx_ctrl.sig_len);
    }
}

esp_err_t wifi_sniffer_init(uint8_t channel, wifi_sniffer_packet_cb_t packet_cb)
{
    esp_err_t ret;

    if (channel < 1 || channel > 13) {
        ESP_LOGE(TAG, "Invalid channel %d", channel);
        return ESP_ERR_INVALID_ARG;
    }

    s_packet_callback = packet_cb;
    s_current_channel = channel;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Set promiscuous mode
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_packet_handler);

    // Set channel
    esp_wifi_set_channel(s_current_channel, WIFI_SECOND_CHAN_NONE);

    ESP_LOGI(TAG, "WiFi sniffer initialized on channel %d", s_current_channel);

    return ESP_OK;
}

esp_err_t wifi_sniffer_start(void)
{
    esp_err_t ret = esp_wifi_set_promiscuous(true);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi sniffer started");
    }
    return ret;
}

esp_err_t wifi_sniffer_stop(void)
{
    esp_err_t ret = esp_wifi_set_promiscuous(false);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi sniffer stopped");
    }
    return ret;
}

esp_err_t wifi_sniffer_set_channel(uint8_t channel)
{
    if (channel < 1 || channel > 13) {
        ESP_LOGE(TAG, "Invalid channel %d", channel);
        return ESP_ERR_INVALID_ARG;
    }

    s_current_channel = channel;
    esp_err_t ret = esp_wifi_set_channel(s_current_channel, WIFI_SECOND_CHAN_NONE);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi channel set to %d", s_current_channel);
    }
    
    return ret;
}

uint8_t wifi_sniffer_get_channel(void)
{
    return s_current_channel;
}
