# WiFi Sniffer with USB Gadget

A WiFi packet sniffer for ESP32-S3 based on ESP-IDF 5.4 that captures WiFi packets in promiscuous mode and streams them via USB CDC-ACM interface.

## Features

- **WiFi Packet Sniffing**: Captures WiFi management, control, and data frames in promiscuous mode
- **USB Gadget Interface**: Outputs captured packets via USB CDC-ACM (appears as a serial port)
- **Channel Hopping**: Automatically cycles through WiFi channels 1-13
- **ESP-IDF 5.4**: Built with the latest ESP-IDF framework
- **ESP32-S3 Support**: Optimized for ESP32-S3 with native USB support

## Hardware Requirements

- **ESP32-S3** development board with USB support (e.g., ESP32-S3-DevKitC-1)
- USB cable for data connection
- WiFi antenna (usually integrated on dev boards)

## Software Requirements

- ESP-IDF v5.4 or later
- Python 3.8 or later
- CMake 3.16 or later

## Installation

### 1. Install ESP-IDF 5.4

Follow the official [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/get-started/index.html) to install ESP-IDF v5.4.

```bash
# Clone ESP-IDF
git clone -b v5.4 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

### 2. Clone This Repository

```bash
git clone https://github.com/JeonghunLee/wifisniffer.git
cd wifisniffer
```

### 3. Build the Project

```bash
# Set the target to ESP32-S3
idf.py set-target esp32s3

# Build the project
idf.py build
```

### 4. Flash to Device

```bash
# Flash the firmware
idf.py -p /dev/ttyUSB0 flash

# Monitor the output (optional)
idf.py -p /dev/ttyUSB0 monitor
```

## Usage

### Connecting to the Device

After flashing, the ESP32-S3 will appear as a USB serial device (CDC-ACM):

- **Linux**: `/dev/ttyACM0` or `/dev/ttyACM1`
- **macOS**: `/dev/cu.usbmodem*`
- **Windows**: `COM3`, `COM4`, etc.

### Viewing Captured Packets

Use any serial terminal program to view the captured packets:

```bash
# Using screen
screen /dev/ttyACM0 115200

# Using minicom
minicom -D /dev/ttyACM0

# Using Python serial
python -m serial.tools.miniterm /dev/ttyACM0 115200

# Using cat (simple viewing)
cat /dev/ttyACM0
```

### Packet Format

Each captured packet is prefixed with metadata:

```
[CH:6 PKT:1234 LEN:128] <packet_data>
```

Where:
- `CH`: WiFi channel (1-13)
- `PKT`: Packet sequence number
- `LEN`: Packet length in bytes

## Architecture

### Components

1. **WiFi Sniffer Module** (`wifi_sniffer.c/h`)
   - Initializes WiFi in promiscuous mode
   - Captures all WiFi packets on specified channel
   - Provides callback interface for packet handling
   - Supports channel switching

2. **USB Gadget Module** (`usb_gadget.c/h`)
   - Implements USB CDC-ACM interface using TinyUSB
   - Provides simple API for sending data over USB
   - Handles USB connection state

3. **Main Application** (`main.c`)
   - Integrates WiFi sniffer and USB gadget
   - Implements channel hopping logic
   - Formats and sends packets to USB

### Data Flow

```
WiFi Packets → Promiscuous Mode Callback → Packet Handler → USB CDC-ACM → Host Computer
```

## Configuration

### WiFi Channel

By default, the sniffer starts on channel 1 and cycles through all channels (1-13) every 5 seconds.

To change the default channel, modify in `main.c`:
```c
#define WIFI_CHANNEL_DEFAULT 6  // Start on channel 6
```

### Channel Switch Interval

To change how often channels are switched, modify in `main.c`:
```c
#define CHANNEL_SWITCH_INTERVAL_MS 10000  // 10 seconds
```

### Disable Channel Hopping

To stay on a single channel, comment out the channel switching task creation in `main.c`:
```c
// xTaskCreate(channel_switch_task, "channel_switch", 2048, NULL, 5, NULL);
```

## API Reference

### WiFi Sniffer API

```c
// Initialize WiFi sniffer
esp_err_t wifi_sniffer_init(uint8_t channel, wifi_sniffer_packet_cb_t packet_cb);

// Start/Stop sniffing
esp_err_t wifi_sniffer_start(void);
esp_err_t wifi_sniffer_stop(void);

// Channel control
esp_err_t wifi_sniffer_set_channel(uint8_t channel);
uint8_t wifi_sniffer_get_channel(void);
```

### USB Gadget API

```c
// Initialize USB gadget
esp_err_t usb_gadget_init(void);

// Send data via USB
esp_err_t usb_gadget_send(const uint8_t *data, uint32_t len);

// Check USB connection status
bool usb_gadget_is_connected(void);
```

## Troubleshooting

### USB Device Not Detected

1. Ensure you're using the **USB port** (not UART port) on ESP32-S3
2. Check if USB cable supports data transfer (not just power)
3. Try a different USB port on your computer
4. Check dmesg/system logs for USB enumeration

### No Packets Captured

1. Verify WiFi antenna is properly connected
2. Check if you're in an area with WiFi traffic
3. Monitor the serial output for errors
4. Ensure promiscuous mode is supported by your ESP32-S3 module

### Build Errors

1. Verify ESP-IDF version: `idf.py --version` should show v5.4.x
2. Clean and rebuild: `idf.py fullclean && idf.py build`
3. Ensure all submodules are initialized: `git submodule update --init --recursive`

## Performance Notes

- Captures up to ~1000 packets per second depending on WiFi traffic
- USB bandwidth sufficient for typical WiFi sniffing scenarios
- Buffer sizes can be adjusted in `sdkconfig.defaults` if needed

## License

This project is provided as-is for educational and research purposes.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## References

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/)
- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [TinyUSB Documentation](https://docs.tinyusb.org/)
- [WiFi Promiscuous Mode](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/network/esp_wifi.html#_CPPv425esp_wifi_set_promiscuousb)