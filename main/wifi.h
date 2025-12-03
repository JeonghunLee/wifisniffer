
#pragma once // wifi.h

#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Wi-Fi 모듈 초기화
 *  - NVS, netif, event loop, wifi_init 등은 이미 다른 곳에서 했다 가정해도 되고
 *  - 최소한 현재 config/mode 를 읽어와 내부 상태(s_sta_config, s_ap_config 등)를 채우는 역할
 */

/**
 * @brief Initialise Wi-Fi manager state by loading NVS-backed configs.
 * @details NVS, netif, event loop, and Wifi driver setup are assumed to be done elsewhere.
 */
esp_err_t wifi_mgr_init(void);
/** @brief Apply the requested Wi-Fi mode (NULL/AP/STA/APSTA). */
esp_err_t wifi_mgr_set_mode(wifi_mode_t mode);         // NULL/AP/STA/APSTA
/** @brief Helper that parses a string such as "sta" or "ap+sta" into @ref wifi_mgr_set_mode. */
esp_err_t wifi_mgr_set_mode_str(const char *mode_str); // "null"/"ap"/"sta"/"apsta"
/** @brief Update the cached STA SSID and apply when appropriate. */
esp_err_t wifi_mgr_set_sta_ssid(const char *ssid);
/** @brief Update the cached STA password and reapply when needed. */
esp_err_t wifi_mgr_set_sta_password(const char *password);
/** @brief Update the SoftAP SSID and reapply when needed. */
esp_err_t wifi_mgr_set_ap_ssid(const char *ssid);
/** @brief Update the SoftAP password (empty string forces open mode). */
esp_err_t wifi_mgr_set_ap_password(const char *password);
/** @brief Change the primary channel while in NULL/AP mode for sniffing. */
esp_err_t wifi_mgr_set_channel(uint8_t primary);

/** @brief Print the current Wi-Fi configuration snapshot to the console. */
void        wifi_mgr_print_status(void);
/** @brief Convert an enum mode to a short human-readable string. */
const char *wifi_mgr_mode_to_string(wifi_mode_t mode);

#ifdef __cplusplus
}
#endif
