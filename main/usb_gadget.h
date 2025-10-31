#ifndef USB_GADGET_H
#define USB_GADGET_H

#include <stdint.h>
#include "esp_err.h"

/**
 * @brief Initialize USB gadget (CDC-ACM)
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t usb_gadget_init(void);

/**
 * @brief Send data via USB
 * 
 * @param data Data buffer
 * @param len Data length
 * @return esp_err_t ESP_OK on success
 */
esp_err_t usb_gadget_send(const uint8_t *data, uint32_t len);

/**
 * @brief Check if USB is connected
 * 
 * @return true if connected
 */
bool usb_gadget_is_connected(void);

#endif // USB_GADGET_H
