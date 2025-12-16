# main/CMakeLists.txt
# ESP-IDF component for Wi-Fi sniffer (TinyUSB CDC + ring buffer)

# ESP32-S3 
set(MAIN_SRCS
    main.c
)

# ESP32-S3 
set(MAIN_HEADERS
)


idf_component_register(
    SRCS   ${MAIN_SRCS}
    INCLUDE_DIRS "."
    REQUIRES 
        wifi_sniffer 
)

# option
target_sources(${COMPONENT_LIB} PRIVATE ${MAIN_HEADERS})