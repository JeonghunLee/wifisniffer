#include "usb_cdc.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "esp_log.h"

static const char *TAG = "USB_CDC";


void usb_cdc_init(void)
{
    // Initialize TinyUSB driver
    tinyusb_config_t tusb_cfg = {
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
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };
    
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    ESP_LOGI(TAG, "USB CDC initialized");
}

bool usb_cdc_ready(void)
{
    // Use TinyUSB's direct API to check connection state
    return tud_cdc_connected();
}

size_t usb_cdc_write(const uint8_t *data, size_t len)
{
    if (!usb_cdc_ready()) {
        return 0;
    }
    
    // Use TinyUSB CDC ACM write function
    return tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, data, len);
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