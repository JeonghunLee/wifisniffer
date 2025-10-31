#include "usb_gadget.h"
#include "esp_log.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include <string.h>

static const char *TAG = "usb_gadget";
static bool s_usb_connected = false;

/**
 * @brief CDC-ACM line state callback
 */
static void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    if (event->type == CDC_EVENT_LINE_STATE_CHANGED) {
        bool dtr = event->line_state_changed_data.dtr;
        bool rts = event->line_state_changed_data.rts;
        s_usb_connected = dtr && rts;
        ESP_LOGI(TAG, "USB Line state changed: DTR=%d, RTS=%d", dtr, rts);
    }
}

esp_err_t usb_gadget_init(void)
{
    ESP_LOGI(TAG, "Initializing USB gadget (CDC-ACM)...");

    // Initialize TinyUSB with default configuration
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    // Initialize CDC-ACM
    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = NULL,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = tinyusb_cdc_line_state_changed_callback,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

    ESP_LOGI(TAG, "USB gadget initialized");

    return ESP_OK;
}

esp_err_t usb_gadget_send(const uint8_t *data, uint32_t len)
{
    if (!s_usb_connected) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t sent = tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, data, len);
    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);

    if (sent != len) {
        ESP_LOGW(TAG, "USB send incomplete: %zu/%lu bytes", sent, len);
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

bool usb_gadget_is_connected(void)
{
    return s_usb_connected;
}
