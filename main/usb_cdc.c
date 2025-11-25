#include "usb_cdc.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "esp_log.h"

static const char *TAG = "USB_CDC";
static bool s_cdc_ready = false;

/* --- line state callback: DTR 올라올 때만 ready --- */
static void line_state_changed_cb(int itf, cdcacm_event_t *event)
{
    (void)itf;

    bool dtr = event->line_state_changed_data.dtr;   // 호스트가 포트 open 하면 1
    bool rts = event->line_state_changed_data.rts;   // 필요하면 같이 사용

    s_cdc_ready = dtr;
    ESP_LOGI(TAG, "line_state: DTR=%d, RTS=%d -> ready=%d", dtr, rts, s_cdc_ready);
}

void usb_cdc_init(void)
{
    // Initialize TinyUSB driver
    
    tinyusb_config_t tusb_cfg = {
        .device_descriptor       = NULL,
        .string_descriptor       = NULL,
        .external_phy            = false,
        .configuration_descriptor= NULL,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    // Initialize CDC-ACM
    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev                        = TINYUSB_USBDEV_0,
        .cdc_port                       = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz               = 64,
        .callback_rx                    = NULL,
        .callback_rx_wanted_char        = NULL,
        .callback_line_state_changed    = line_state_changed_cb,
        .callback_line_coding_changed   = NULL,
    };
    
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    ESP_LOGI(TAG, "USB CDC initialized");
}

bool usb_cdc_ready(void)
{
    // 호스트가 실제로 포트를 연 상태인지 + TinyUSB 연결 여부
    // Use TinyUSB's direct API to check connection state
    return s_cdc_ready && tud_cdc_connected();
}

size_t usb_cdc_write(const uint8_t *data, size_t len)
{
    if (!usb_cdc_ready()) {
        return 0;
    }
    
    // Use TinyUSB CDC ACM write function
    size_t queued = tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, data, len);
    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);  // 바로 전송    
    return queued;
}
size_t usb_cdc_read(uint8_t *data, size_t len)
{
    size_t rx_size = 0;
    esp_err_t ret = tinyusb_cdcacm_read(TINYUSB_CDC_ACM_0, data, len, &rx_size);
    
    if (ret != ESP_OK) {
        return 0;
    }
    
    return rx_size;
}

void usb_cdc_flush(void)
{
    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
}