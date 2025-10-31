/*
 * ESP32-S3 WiFi Sniffer for Wireshark
 * 
 * This program captures WiFi packets in promiscuous mode and outputs them
 * in pcap format over serial, which can be piped to Wireshark.
 * 
 * Supports channel hopping and filtering.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_timer.h"

static const char *TAG = "WiFiSniffer";

// Configuration
#define CHANNEL_HOP_INTERVAL_MS 500  // Time to stay on each channel
#define MAX_CHANNEL 13               // Maximum WiFi channel to scan
#define CHANNEL_HOP_ENABLED 1        // Set to 0 to stay on one channel

// Current channel being monitored
static uint8_t current_channel = 1;

// Pcap file header structure
typedef struct {
    uint32_t magic_number;   // 0xa1b2c3d4
    uint16_t version_major;  // 2
    uint16_t version_minor;  // 4
    int32_t  thiszone;       // GMT to local correction
    uint32_t sigfigs;        // accuracy of timestamps
    uint32_t snaplen;        // max length of captured packets
    uint32_t network;        // data link type (105 = IEEE 802.11)
} pcap_file_header_t;

// Pcap packet header structure
typedef struct {
    uint32_t ts_sec;         // timestamp seconds
    uint32_t ts_usec;        // timestamp microseconds
    uint32_t incl_len;       // number of octets saved in file
    uint32_t orig_len;       // actual length of packet
} pcap_packet_header_t;

// Radiotap header structure for additional radio information
typedef struct {
    uint8_t it_version;      // Radiotap version
    uint8_t it_pad;
    uint16_t it_len;         // Length of radiotap header
    uint32_t it_present;     // Present flags
} __attribute__((packed)) radiotap_header_t;

// Function to write pcap file header to stdout
static void write_pcap_header(void) {
    pcap_file_header_t header = {
        .magic_number = 0xa1b2c3d4,
        .version_major = 2,
        .version_minor = 4,
        .thiszone = 0,
        .sigfigs = 0,
        .snaplen = 65535,
        .network = 127  // 127 = Radiotap header + 802.11
    };
    
    fwrite(&header, sizeof(header), 1, stdout);
    fflush(stdout);
}

// Function to write a packet in pcap format
static void write_pcap_packet(const uint8_t *data, uint32_t len, uint32_t seconds, uint32_t microseconds) {
    // Create radiotap header (minimal version)
    radiotap_header_t radiotap = {
        .it_version = 0,
        .it_pad = 0,
        .it_len = 8,  // 8 bytes for minimal radiotap header
        .it_present = 0  // No additional fields
    };
    
    // Calculate total length
    uint32_t total_len = sizeof(radiotap) + len;
    
    // Write pcap packet header
    pcap_packet_header_t pkt_header = {
        .ts_sec = seconds,
        .ts_usec = microseconds,
        .incl_len = total_len,
        .orig_len = total_len
    };
    
    fwrite(&pkt_header, sizeof(pkt_header), 1, stdout);
    
    // Write radiotap header
    fwrite(&radiotap, sizeof(radiotap), 1, stdout);
    
    // Write actual packet data
    fwrite(data, len, 1, stdout);
    
    fflush(stdout);
}

// WiFi promiscuous mode callback
static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    // Get timestamp
    int64_t timestamp = esp_timer_get_time();
    uint32_t seconds = timestamp / 1000000;
    uint32_t microseconds = timestamp % 1000000;
    
    // Write packet to stdout in pcap format
    write_pcap_packet(pkt->payload, pkt->rx_ctrl.sig_len, seconds, microseconds);
}

// Task to hop between WiFi channels
static void channel_hop_task(void *pvParameter) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_HOP_INTERVAL_MS));
        
#if CHANNEL_HOP_ENABLED
        current_channel = (current_channel % MAX_CHANNEL) + 1;
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(TAG, "Hopped to channel %d", current_channel);
#endif
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "WiFi Sniffer for ESP32-S3 Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    
    // Initialize event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // Set WiFi promiscuous mode
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    
    // Register promiscuous callback
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_callback));
    
    // Set initial channel
    ESP_ERROR_CHECK(esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE));
    
    ESP_LOGI(TAG, "WiFi Sniffer started on channel %d", current_channel);
    ESP_LOGI(TAG, "Channel hopping: %s", CHANNEL_HOP_ENABLED ? "ENABLED" : "DISABLED");
    
    // Write pcap file header
    write_pcap_header();
    
    ESP_LOGI(TAG, "Pcap header written, starting packet capture...");
    
    // Create channel hopping task
    xTaskCreate(channel_hop_task, "channel_hop", 2048, NULL, 5, NULL);
    
    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
