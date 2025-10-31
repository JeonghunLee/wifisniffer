#ifndef WIFI_SNIFFER_H
#define WIFI_SNIFFER_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_wifi_types.h"

/**
 * @brief WiFi packet callback function type
 * 
 * @param buf Packet buffer
 * @param len Packet length
 */
typedef void (*wifi_sniffer_packet_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief Initialize WiFi sniffer
 * 
 * @param channel WiFi channel to sniff (1-13)
 * @param packet_cb Callback function for received packets
 * @return esp_err_t ESP_OK on success
 */
esp_err_t wifi_sniffer_init(uint8_t channel, wifi_sniffer_packet_cb_t packet_cb);

/**
 * @brief Start WiFi sniffing
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t wifi_sniffer_start(void);

/**
 * @brief Stop WiFi sniffing
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t wifi_sniffer_stop(void);

/**
 * @brief Set WiFi channel
 * 
 * @param channel WiFi channel (1-13)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t wifi_sniffer_set_channel(uint8_t channel);

/**
 * @brief Get current WiFi channel
 * 
 * @return uint8_t Current channel
 */
uint8_t wifi_sniffer_get_channel(void);

#endif // WIFI_SNIFFER_H
