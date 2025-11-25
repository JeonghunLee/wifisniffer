# main/CMakeLists.txt
# ESP-IDF component for Wi-Fi sniffer (TinyUSB CDC + ring buffer)

# ESP32-S3 
set(SNIFFER_SRCS
    main.c
    cmd.c
    wifi.c
    sniffer.c
    usb_cdc.c
    ring_buf.c
)

# ESP32-S3 
set(SNIFFER_HEADERS
    cmd.h
    wifi.h    
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
        console
)

# option
target_sources(${COMPONENT_LIB} PRIVATE ${SNIFFER_HEADERS})