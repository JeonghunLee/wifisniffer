#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

/* TinyUSB CDC interface */
/** @brief Initialise TinyUSB CDC-ACM driver and install callbacks. */
void usb_cdc_init(void);
/** @brief Check whether the host has opened the CDC port and asserted DTR. */
bool usb_cdc_ready(void);
/** @brief Queue bytes for transmission to the host over the CDC endpoint. */
size_t usb_cdc_write(const uint8_t *data, size_t len);
/** @brief Read any pending bytes from the host. */
size_t usb_cdc_read(uint8_t *data, size_t len);
/** @brief Flush any queued TX data to the host immediately. */
void usb_cdc_flush(void);
