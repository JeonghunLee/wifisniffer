
#pragma once // wifi.h

#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Wi-Fi 모듈 초기화
 *  - NVS, netif, event loop, wifi_init 등은 이미 다른 곳에서 했다 가정해도 되고
 *  - 최소한 현재 config/mode 를 읽어와 내부 상태(s_sta_config, s_ap_config 등)를 채우는 역할
 */
esp_err_t wifi_mgr_init(void);
esp_err_t wifi_mgr_set_mode(wifi_mode_t mode);         // NULL/AP/STA/APSTA
esp_err_t wifi_mgr_set_mode_str(const char *mode_str); // "null"/"ap"/"sta"/"apsta"
esp_err_t wifi_mgr_set_sta_ssid(const char *ssid);
esp_err_t wifi_mgr_set_sta_password(const char *password);
esp_err_t wifi_mgr_set_ap_ssid(const char *ssid);
esp_err_t wifi_mgr_set_ap_password(const char *password);
esp_err_t wifi_mgr_set_channel(uint8_t primary);

void        wifi_mgr_print_status(void);
const char *wifi_mgr_mode_to_string(wifi_mode_t mode);

#ifdef __cplusplus
}
#endif
