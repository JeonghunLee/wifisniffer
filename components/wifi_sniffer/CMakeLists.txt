# components/wifi_sniffer/CMakeLists.txt
# ESP-IDF component for Wi-Fi sniffer (TinyUSB CDC + ring buffer)

# ESP32-S3 
set(SNIFFER_SRCS
    wifi_sniffer.c
    cmd.c
    wifi.c
    sniffer.c
    usb_cdc.c
    ring_buf.c
)

# ESP32-S3 
set(SNIFFER_HEADERS
    wifi_sniffer.h
    cmd.h
    wifi.h    
    sniffer.h
    usb_cdc.h
    ring_buf.h
)

#
# ESP32-S3 WIFI SNIFFER component
#       tinyusb CDC + ring buffer
#       esp_wifi nvs_flash esp_netif esp_event console
idf_component_register(
    SRCS  ${SNIFFER_SRCS}
    INCLUDE_DIRS "."
    REQUIRES 
        tinyusb 
        esp_wifi 
        nvs_flash
        esp_netif 
        esp_event
        console
)

