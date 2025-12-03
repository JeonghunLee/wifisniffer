#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

/* TinyUSB CDC interface */
void usb_cdc_init(void);
bool usb_cdc_ready(void);
size_t usb_cdc_write(const uint8_t *data, size_t len);
size_t usb_cdc_read(uint8_t *data, size_t len);
void usb_cdc_flush(void);
