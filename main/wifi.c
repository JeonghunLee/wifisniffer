// wifi.c
#include "wifi.h"

#include <string.h>
#include <stdio.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "sniffer.h"   

static const char *TAG = "WIFI_MGR";

//  wifi manager 내부 상태
static wifi_config_t s_sta_config;
static wifi_config_t s_ap_config;
static wifi_mode_t   s_current_mode = WIFI_MODE_NULL;
static bool          s_initialized  = false;

// Forward declarations
static void ensure_ap_defaults(void);
static esp_err_t apply_wifi_mode_internal(wifi_mode_t mode);
static esp_err_t update_sta_ssid(const char *ssid);
static esp_err_t update_sta_password(const char *password);
static esp_err_t update_ap_ssid(const char *ssid);
static esp_err_t update_ap_password(const char *password);

// WIFI AP MODE defaults
#define WIFI_AP_DEFAULT_CHANNEL         1
#define WIFI_AP_DEFAULT_MAX_CONNECTION  4
#define WIFI_AP_DEFAULT_BEACON_INTERVAL 100

/**
 * AP 설정 기본값 보장
 */
static void ensure_ap_defaults(void)
{
    if (s_ap_config.ap.channel == 0) {
        s_ap_config.ap.channel = WIFI_AP_DEFAULT_CHANNEL;
    }
    if (s_ap_config.ap.max_connection == 0) {
        s_ap_config.ap.max_connection = WIFI_AP_DEFAULT_MAX_CONNECTION;
    }
    if (s_ap_config.ap.beacon_interval == 0) {
        s_ap_config.ap.beacon_interval = WIFI_AP_DEFAULT_BEACON_INTERVAL;
    }
}

/**
 * 모드 문자열을 Wi-Fi 모드로 변환
 */
const char *wifi_mgr_mode_to_string(wifi_mode_t mode)
{
    switch (mode) {
        case WIFI_MODE_NULL: return "null";
        case WIFI_MODE_STA:  return "sta";
        case WIFI_MODE_AP:   return "ap";
        case WIFI_MODE_APSTA:return "ap+sta";
        default:             return "unknown";
    }
}


esp_err_t wifi_mgr_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }
    s_initialized = true;

    memset(&s_sta_config, 0, sizeof(s_sta_config));
    memset(&s_ap_config, 0, sizeof(s_ap_config));

    (void)esp_wifi_get_config(WIFI_IF_STA, &s_sta_config);
    if (esp_wifi_get_config(WIFI_IF_AP, &s_ap_config) != ESP_OK) {
        memset(&s_ap_config, 0, sizeof(s_ap_config));
    }
    ensure_ap_defaults();
    (void)esp_wifi_get_mode(&s_current_mode);

    ESP_LOGI(TAG, "wifi_mgr_init: mode=%s", wifi_mgr_mode_to_string(s_current_mode));
    return ESP_OK;
}


esp_err_t wifi_mgr_set_mode(wifi_mode_t mode)
{
    ESP_ERROR_CHECK(wifi_mgr_init());
    return apply_wifi_mode_internal(mode);
}

esp_err_t wifi_mgr_set_mode_str(const char *mode_str)
{
    if (mode_str == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_mode_t mode = WIFI_MODE_NULL;

    if (strcasecmp(mode_str, "null") == 0) {
        mode = WIFI_MODE_NULL;
    }
    else if (strcasecmp(mode_str, "sta") == 0) {
        mode = WIFI_MODE_STA;
    }
    else if (strcasecmp(mode_str, "ap") == 0) {
        mode = WIFI_MODE_AP;
    }
    else if (strcasecmp(mode_str, "ap+sta") == 0 ||
             strcasecmp(mode_str, "apsta") == 0 ||
             strcasecmp(mode_str, "sta+ap") == 0) {
        mode = WIFI_MODE_APSTA;
    }
    else {
        return ESP_ERR_INVALID_ARG;
    }

    // 모드 적용
    esp_err_t err = wifi_mgr_set_mode(mode);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}


esp_err_t wifi_mgr_set_sta_ssid(const char *ssid)
{
    ESP_ERROR_CHECK(wifi_mgr_init());
    esp_err_t err = update_sta_ssid(ssid);
    if (err == ESP_OK &&
        (s_current_mode == WIFI_MODE_STA || s_current_mode == WIFI_MODE_APSTA)) {
        sniffer_enable_promiscuous();
    }
    return err;
}

esp_err_t wifi_mgr_set_sta_password(const char *password)
{
    ESP_ERROR_CHECK(wifi_mgr_init());
    esp_err_t err = update_sta_password(password);
    if (err == ESP_OK &&
        (s_current_mode == WIFI_MODE_STA || s_current_mode == WIFI_MODE_APSTA)) {
        sniffer_enable_promiscuous();
    }
    return err;
}

esp_err_t wifi_mgr_set_ap_ssid(const char *ssid)
{
    ESP_ERROR_CHECK(wifi_mgr_init());
    esp_err_t err = update_ap_ssid(ssid);
    if (err == ESP_OK &&
        (s_current_mode == WIFI_MODE_AP || s_current_mode == WIFI_MODE_APSTA)) {
        sniffer_enable_promiscuous();
    }
    return err;
}

esp_err_t wifi_mgr_set_ap_password(const char *password)
{
    ESP_ERROR_CHECK(wifi_mgr_init());
    esp_err_t err = update_ap_password(password);
    if (err == ESP_OK &&
        (s_current_mode == WIFI_MODE_AP || s_current_mode == WIFI_MODE_APSTA)) {
        sniffer_enable_promiscuous();
    }
    return err;
}

esp_err_t wifi_mgr_set_channel(uint8_t primary)
{
    wifi_mode_t mode = WIFI_MODE_NULL;

    if (primary < 1 || primary > 13) {   // 필요하면 14까지 허용
        return ESP_ERR_INVALID_ARG;
    }

    ESP_ERROR_CHECK(wifi_mgr_init());
    (void)esp_wifi_get_mode(&mode);

    // 1) STA / APSTA인 경우: 경고 또는 막기
    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) {
        // 여기서 정책 결정:
        //   - 그냥 막을지
        //   - 또는 경고만 찍고 시도할지
        ESP_LOGW(TAG,
                 "Channel change in STA/APSTA mode may be invalid or break connection. "
                 "Use NULL/AP mode for sniffer channel hopping.");
        // 나는 아예 막는 쪽이 더 안전하다고 봄
        return ESP_ERR_INVALID_STATE;
    }    
    // 2) NULL / AP 모드인 경우: 채널 변경 시도
    esp_err_t err = esp_wifi_set_channel(primary, WIFI_SECOND_CHAN_NONE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_channel failed: %s", esp_err_to_name(err));
        return err;
    }

    // 채널 변경 후 스니퍼 다시 세팅 (필요시)
    sniffer_enable_promiscuous();

    ESP_LOGI(TAG, "Wi-Fi channel set to %u", primary);
    return ESP_OK;
}


void wifi_mgr_print_status(void)
{
    wifi_mode_t mode = WIFI_MODE_NULL;
    (void)esp_wifi_get_mode(&mode);

    wifi_config_t sta;
    wifi_config_t ap;
    memset(&sta, 0, sizeof(sta));
    memset(&ap, 0, sizeof(ap));

    if (esp_wifi_get_config(WIFI_IF_STA, &sta) != ESP_OK) {
        sta = s_sta_config;
    }
    if (esp_wifi_get_config(WIFI_IF_AP, &ap) != ESP_OK) {
        ap = s_ap_config;
    }

    printf("Wi-Fi mode: %s\n", wifi_mgr_mode_to_string(mode));
    printf("-  STA SSID: %s\n", sta.sta.ssid[0] ? (char *)sta.sta.ssid : "<unset>");
    printf("-  STA password: %s\n", sta.sta.password[0] ? "<set>" : "<empty>");
    printf("-  AP SSID: %s\n", ap.ap.ssid[0] ? (char *)ap.ap.ssid : "<unset>");
    printf("-  AP password: %s\n",
           (ap.ap.authmode == WIFI_AUTH_OPEN) ? "<open>" :
           (ap.ap.password[0] ? "<set>" : "<invalid>"));
    printf("-  AP channel: %u\n", ap.ap.channel);
}

// Wi-Fi 모드 적용 내부 함수
static esp_err_t apply_wifi_mode_internal(wifi_mode_t mode)
{
    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK &&
        err != ESP_ERR_WIFI_NOT_INIT &&
        err != ESP_ERR_WIFI_NOT_STARTED) {
        ESP_LOGE(TAG, "esp_wifi_stop failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_mode(mode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return err;
    }

    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) {
        err = esp_wifi_set_config(WIFI_IF_STA, &s_sta_config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "STA config failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA) {
        wifi_config_t cfg = s_ap_config;
        cfg.ap.ssid_len = (uint8_t)strnlen((const char *)cfg.ap.ssid,
                                           sizeof(cfg.ap.ssid));
        size_t pwd_len = strnlen((const char *)cfg.ap.password,
                                 sizeof(cfg.ap.password));
        cfg.ap.authmode = (pwd_len >= 8) ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
        if (cfg.ap.authmode == WIFI_AUTH_OPEN) {
            memset(cfg.ap.password, 0, sizeof(cfg.ap.password));
        }
        ensure_ap_defaults();
        err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "AP config failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return err;
    }

    s_current_mode = mode;

    // 여기서 스니퍼용 promiscuous 모드 활성화
    sniffer_enable_promiscuous();

    ESP_LOGI(TAG, "Wi-Fi mode set to %s", wifi_mgr_mode_to_string(mode));
    return ESP_OK;
}

static esp_err_t update_sta_ssid(const char *ssid)
{
    size_t len = strlen(ssid);
    if (len == 0 || len > sizeof(s_sta_config.sta.ssid)) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(s_sta_config.sta.ssid, 0, sizeof(s_sta_config.sta.ssid));
    memcpy(s_sta_config.sta.ssid, ssid, len);
    return esp_wifi_set_config(WIFI_IF_STA, &s_sta_config);
}

static esp_err_t update_sta_password(const char *password)
{
    size_t len = strlen(password);
    if (len > sizeof(s_sta_config.sta.password) - 1) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(s_sta_config.sta.password, 0, sizeof(s_sta_config.sta.password));
    memcpy(s_sta_config.sta.password, password, len);
    return esp_wifi_set_config(WIFI_IF_STA, &s_sta_config);
}

static esp_err_t update_ap_ssid(const char *ssid)
{
    size_t len = strlen(ssid);
    if (len == 0 || len > sizeof(s_ap_config.ap.ssid)) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(s_ap_config.ap.ssid, 0, sizeof(s_ap_config.ap.ssid));
    memcpy(s_ap_config.ap.ssid, ssid, len);
    s_ap_config.ap.ssid_len = (uint8_t)len;
    ensure_ap_defaults();
    return esp_wifi_set_config(WIFI_IF_AP, &s_ap_config);
}

static esp_err_t update_ap_password(const char *password)
{
    size_t len = strlen(password);
    if (len == 0) {
        memset(s_ap_config.ap.password, 0, sizeof(s_ap_config.ap.password));
        s_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    } else {
        if (len < 8 || len > sizeof(s_ap_config.ap.password) - 1) {
            return ESP_ERR_INVALID_ARG;
        }
        memset(s_ap_config.ap.password, 0, sizeof(s_ap_config.ap.password));
        memcpy(s_ap_config.ap.password, password, len);
        s_ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    }
    ensure_ap_defaults();
    wifi_config_t cfg = s_ap_config;
    cfg.ap.ssid_len = (uint8_t)strnlen((const char *)cfg.ap.ssid,
                                       sizeof(cfg.ap.ssid));
    return esp_wifi_set_config(WIFI_IF_AP, &cfg);
}



