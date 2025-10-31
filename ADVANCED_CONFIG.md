# Advanced Configuration Guide

This guide covers advanced configuration options for customizing the ESP32-S3 WiFi sniffer.

## Table of Contents

1. [WiFi Configuration](#wifi-configuration)
2. [Serial Configuration](#serial-configuration)
3. [Performance Tuning](#performance-tuning)
4. [Custom Channel Configurations](#custom-channel-configurations)
5. [Packet Filtering](#packet-filtering)
6. [Power Management](#power-management)

## WiFi Configuration

### Buffer Sizes

Edit `sdkconfig.defaults`:

```ini
# Static RX buffers (allocated at init)
CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM=16

# Dynamic RX buffers (allocated as needed)
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=32

# Dynamic TX buffers
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=32

# Increase for high-traffic environments
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=64
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=64
```

### AMPDU Configuration

Aggregate MAC Protocol Data Unit settings:

```ini
# Enable/disable RX AMPDU
CONFIG_ESP_WIFI_AMPDU_RX_ENABLED=y

# Enable/disable TX AMPDU
CONFIG_ESP_WIFI_AMPDU_TX_ENABLED=y

# Max AMPDU length
CONFIG_ESP_WIFI_RX_BA_WIN=6
```

### WiFi Task Priority

For better responsiveness:

```ini
# WiFi task priority (higher = more priority)
CONFIG_ESP_WIFI_TASK_CORE_ID=0
CONFIG_ESP_WIFI_TASK_PRIORITY=23
```

## Serial Configuration

### Baudrate Options

Higher baudrates allow more throughput but may be less stable:

```ini
# Standard rates (most reliable)
CONFIG_ESP_CONSOLE_UART_BAUDRATE=115200
CONFIG_ESP_CONSOLE_UART_BAUDRATE=460800
CONFIG_ESP_CONSOLE_UART_BAUDRATE=921600

# High-speed rates (requires good cable)
CONFIG_ESP_CONSOLE_UART_BAUDRATE=1500000
CONFIG_ESP_CONSOLE_UART_BAUDRATE=2000000
CONFIG_ESP_CONSOLE_UART_BAUDRATE=3000000
```

**Note**: Update Python scripts to match:
```python
BAUDRATE = 921600  # Match sdkconfig.defaults
```

### UART Configuration

For advanced UART tuning, edit `main/wifisniffer.c`:

```c
// Add UART configuration
#include "driver/uart.h"

void configure_uart(void) {
    uart_config_t uart_config = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_tx_buffer(UART_NUM_0, 1024 * 4);
}

// Call in app_main() before packet capture
```

## Performance Tuning

### CPU Frequency

Higher CPU frequency improves packet processing:

```ini
# Options: 80, 160, 240 (MHz)
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_240=y
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y
```

### FreeRTOS Configuration

```ini
# Tick rate (higher = more responsive)
CONFIG_FREERTOS_HZ=1000

# Enable CO-processor
CONFIG_ESP32S3_SPIRAM_SUPPORT=y
```

### Memory Optimization

```ini
# Use PSRAM for large buffers
CONFIG_ESP32S3_SPIRAM_SUPPORT=y
CONFIG_SPIRAM_USE_MALLOC=y
CONFIG_SPIRAM_MODE_QUAD=y
CONFIG_SPIRAM_SPEED_80M=y
```

## Custom Channel Configurations

### Monitor Specific Channels Only

Edit `main/wifisniffer.c`:

```c
// Non-overlapping 2.4GHz channels
static void channel_hop_task(void *pvParameter) {
    uint8_t channels[] = {1, 6, 11};
    uint8_t num_channels = 3;
    uint8_t idx = 0;
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_HOP_INTERVAL_MS));
        idx = (idx + 1) % num_channels;
        current_channel = channels[idx];
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(TAG, "Hopped to channel %d", current_channel);
    }
}
```

### Adaptive Channel Hopping

Stay longer on busy channels:

```c
#define MIN_HOP_INTERVAL_MS 200
#define MAX_HOP_INTERVAL_MS 2000

static uint32_t packets_on_channel = 0;

static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    packets_on_channel++;
    
    // ... existing packet handling code ...
}

static void channel_hop_task(void *pvParameter) {
    while (1) {
        uint32_t interval = MAX_HOP_INTERVAL_MS;
        
        // Stay longer on busy channels
        if (packets_on_channel > 100) {
            interval = MIN_HOP_INTERVAL_MS;
        } else if (packets_on_channel > 50) {
            interval = MIN_HOP_INTERVAL_MS * 2;
        }
        
        vTaskDelay(pdMS_TO_TICKS(interval));
        
        packets_on_channel = 0;
        current_channel = (current_channel % MAX_CHANNEL) + 1;
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
    }
}
```

### Include 5GHz Channels (ESP32-S3 Specific)

**Note**: This requires hardware that supports 5GHz.

```c
// Extended channel list (2.4GHz + 5GHz)
#define MAX_CHANNEL_24GHZ 13
#define MAX_CHANNEL_5GHZ 165

static void channel_hop_task(void *pvParameter) {
    // 5GHz channels (selected common ones)
    uint8_t channels_5ghz[] = {36, 40, 44, 48, 149, 153, 157, 161, 165};
    uint8_t num_5ghz = sizeof(channels_5ghz) / sizeof(channels_5ghz[0]);
    
    uint8_t idx_24 = 0;
    uint8_t idx_5 = 0;
    bool use_5ghz = false;
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_HOP_INTERVAL_MS));
        
        if (use_5ghz) {
            current_channel = channels_5ghz[idx_5];
            idx_5 = (idx_5 + 1) % num_5ghz;
        } else {
            current_channel = (idx_24 % MAX_CHANNEL_24GHZ) + 1;
            idx_24++;
        }
        
        use_5ghz = !use_5ghz;  // Alternate between 2.4GHz and 5GHz
        
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(TAG, "Channel %d (%s)", current_channel, 
                 use_5ghz ? "5GHz" : "2.4GHz");
    }
}
```

## Packet Filtering

### Filter by Frame Type

```c
static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    // Filter: only capture management frames
    if (type != WIFI_PKT_MGMT) {
        return;
    }
    
    // ... rest of callback code ...
}
```

### Filter by Signal Strength

```c
#define MIN_RSSI -80  // Only capture packets with RSSI > -80 dBm

static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    // Filter by signal strength
    if (pkt->rx_ctrl.rssi < MIN_RSSI) {
        return;
    }
    
    // ... rest of callback code ...
}
```

### Filter by MAC Address

```c
static uint8_t target_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    // 802.11 frame format: [FC][Duration][Addr1][Addr2][Addr3]...
    uint8_t *mac_addr1 = &pkt->payload[4];  // Destination address
    uint8_t *mac_addr2 = &pkt->payload[10]; // Source address
    
    // Filter: only packets to/from target MAC
    if (memcmp(mac_addr1, target_mac, 6) != 0 && 
        memcmp(mac_addr2, target_mac, 6) != 0) {
        return;
    }
    
    // ... rest of callback code ...
}
```

### Enhanced Radiotap Header

Add more radio information:

```c
typedef struct {
    uint8_t it_version;
    uint8_t it_pad;
    uint16_t it_len;
    uint32_t it_present;
    
    // Additional fields when present flags are set
    uint64_t timestamp;      // Present flag: 0x0001
    uint8_t flags;           // Present flag: 0x0002
    uint8_t rate;            // Present flag: 0x0004
    uint16_t channel_freq;   // Present flag: 0x0008
    uint16_t channel_flags;  // Present flag: 0x0008
    int8_t antenna_signal;   // Present flag: 0x0020
} __attribute__((packed)) radiotap_header_enhanced_t;

static void write_pcap_packet_enhanced(const uint8_t *data, uint32_t len, 
                                       uint32_t seconds, uint32_t microseconds,
                                       wifi_promiscuous_pkt_t *pkt) {
    radiotap_header_enhanced_t radiotap = {
        .it_version = 0,
        .it_pad = 0,
        .it_len = sizeof(radiotap_header_enhanced_t),
        .it_present = 0x0000002A,  // timestamp, flags, rate, antenna_signal
        .timestamp = esp_timer_get_time(),
        .flags = 0,
        .rate = pkt->rx_ctrl.rate,
        .channel_freq = 2407 + (current_channel * 5),
        .channel_flags = 0x00A0,  // 2GHz spectrum, dynamic CCK-OFDM
        .antenna_signal = pkt->rx_ctrl.rssi
    };
    
    // ... write packet with enhanced header ...
}
```

## Power Management

### Deep Sleep Between Captures

For battery-powered applications:

```c
#include "esp_sleep.h"

#define CAPTURE_DURATION_MS 10000  // 10 seconds
#define SLEEP_DURATION_MS 50000    // 50 seconds

void app_main(void) {
    // ... initialization code ...
    
    // Capture packets
    vTaskDelay(pdMS_TO_TICKS(CAPTURE_DURATION_MS));
    
    // Flush buffers
    fflush(stdout);
    
    // Enter deep sleep
    ESP_LOGI(TAG, "Entering deep sleep...");
    esp_deep_sleep(SLEEP_DURATION_MS * 1000);
}
```

### Light Sleep Between Channel Hops

Reduce power consumption:

```c
#include "esp_pm.h"

void configure_power_management(void) {
    esp_pm_config_esp32s3_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 80,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}
```

## Compile-Time Options

Create custom build targets in `CMakeLists.txt`:

```cmake
# Add compile definitions
if(CONFIG_WIFI_SNIFFER_CHANNEL_HOP)
    target_compile_definitions(${COMPONENT_TARGET} PRIVATE CHANNEL_HOP_ENABLED=1)
else()
    target_compile_definitions(${COMPONENT_TARGET} PRIVATE CHANNEL_HOP_ENABLED=0)
endif()
```

Then use in code:

```c
#ifndef CHANNEL_HOP_ENABLED
#define CHANNEL_HOP_ENABLED 1
#endif
```

## Logging and Debugging

### Enable Verbose WiFi Logs

Edit `sdkconfig.defaults`:

```ini
# Enable verbose logging
CONFIG_LOG_DEFAULT_LEVEL_VERBOSE=y
CONFIG_LOG_DEFAULT_LEVEL=5

# WiFi-specific logging
CONFIG_ESP_WIFI_DEBUG_LOG_ENABLE=y
CONFIG_ESP_WIFI_DEBUG_LOG_VERBOSE=y
```

### Custom Debug Output

Add debug information without affecting pcap output:

```c
// Use stderr for debug messages (stdout is for pcap data)
#define DEBUG_LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

static void wifi_sniffer_callback(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;
    
    DEBUG_LOG("PKT: type=%d len=%d rssi=%d", type, pkt->rx_ctrl.sig_len, pkt->rx_ctrl.rssi);
    
    // ... normal packet handling ...
}
```

## Building Custom Configurations

Create configuration profiles:

```bash
# Profile 1: Maximum performance
cat > sdkconfig.performance << EOF
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=64
CONFIG_ESP_CONSOLE_UART_BAUDRATE=2000000
EOF

# Profile 2: Power efficient
cat > sdkconfig.power_efficient << EOF
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_160=y
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=16
CONFIG_ESP_CONSOLE_UART_BAUDRATE=460800
CONFIG_PM_ENABLE=y
EOF

# Use specific profile
cp sdkconfig.performance sdkconfig.defaults
idf.py build
```

## Testing Configurations

Validate changes:

```bash
# Monitor with statistics
idf.py -p /dev/ttyUSB0 flash monitor

# Capture test data
timeout 60 python3 tools/advanced_sniffer.py /dev/ttyUSB0 -s -o test.pcap

# Analyze capture
tshark -r test.pcap -q -z io,stat,1
```

## Performance Benchmarks

Typical performance with different configurations:

| Configuration | Baudrate | CPU | Packets/sec | Comments |
|--------------|----------|-----|-------------|----------|
| Default | 921600 | 240MHz | ~500 | Good balance |
| High Performance | 2000000 | 240MHz | ~1000 | Requires good cable |
| Power Efficient | 460800 | 160MHz | ~300 | Lower power consumption |
| Single Channel | 921600 | 240MHz | ~800 | No channel hopping |

## Further Customization

For even more control, refer to:
- [ESP-IDF WiFi API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/network/esp_wifi.html)
- [ESP-IDF Configuration Options](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/kconfig.html)
- [FreeRTOS Documentation](https://www.freertos.org/a00106.html)
