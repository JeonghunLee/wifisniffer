# main/CMakeLists.txt
# ESP-IDF component for Wi-Fi sniffer (TinyUSB CDC + ring buffer)

# ESP32-S3 
set(SNIFFER_SRCS
    sniffer.c
    usb_cdc.c
    ring_buf.c
)

set(SNIFFER_HEADERS
    sniffer.h
    usb_cdc.h
    ring_buf.h
)


idf_component_register(
    SRCS  ${SNIFFER_SRCS}
    INCLUDE_DIRS "."
    REQUIRES 
        tinyusb 
        esp_wifi 
        nvs_flash
        esp_netif 
        esp_event
)

# option
target_sources(${COMPONENT_LIB} PRIVATE ${SNIFFER_HEADERS})