# WiFi Sniffer for ESP32-S3 DevKit-C1

A WiFi packet sniffer for ESP32-S3 DevKit-C1 that captures 802.11 packets in promiscuous mode and streams them in pcap format to Wireshark for real-time analysis.

## Features

- **Real-time packet capture**: Captures WiFi packets in promiscuous mode
- **Wireshark integration**: Outputs packets in pcap format compatible with Wireshark
- **Channel hopping**: Automatically scans across WiFi channels (1-13)
- **High performance**: Optimized for ESP32-S3 with 921600 baud serial
- **Easy to use**: Simple Python scripts for capture and analysis

## Hardware Requirements

- ESP32-S3 DevKit-C1 board
- USB cable for power and data
- Computer with USB port

## Software Requirements

### ESP32 Development
- ESP-IDF v4.4 or later (v5.x recommended)
- USB-to-UART driver for your ESP32-S3 board

### PC Software
- Python 3.6 or later
- pyserial library (`pip install pyserial`)
- Wireshark (for live capture)

## Quick Start

### 1. Install ESP-IDF

Follow the official ESP-IDF installation guide:
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/)

### 2. Build and Flash

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure for ESP32-S3
idf.py set-target esp32s3

# Build the project
idf.py build

# Flash to your ESP32-S3 DevKit-C1
idf.py -p /dev/ttyUSB0 flash

# Note: Replace /dev/ttyUSB0 with your serial port (COM3 on Windows)
```

### 3. Capture Packets

#### Option A: Live Capture with Wireshark

```bash
# Install Python dependencies
pip install pyserial

# Run the bridge script
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0

# On Windows:
python3 tools/sniffer_to_wireshark.py COM3
```

This will automatically open Wireshark and start displaying captured packets in real-time.

#### Option B: Capture to File

```bash
# Capture to a pcap file
python3 tools/capture_to_file.py /dev/ttyUSB0 capture.pcap

# Open the file in Wireshark when done
wireshark capture.pcap
```

## Configuration

### Change WiFi Channel

By default, the sniffer hops between channels 1-13. To monitor a specific channel:

1. Edit `main/wifisniffer.c`
2. Set `CHANNEL_HOP_ENABLED` to `0`
3. Set the desired channel in `current_channel` variable
4. Rebuild and flash

```c
#define CHANNEL_HOP_ENABLED 0        // Disable hopping
static uint8_t current_channel = 6;  // Monitor channel 6
```

### Adjust Channel Hop Interval

To change how long the sniffer stays on each channel:

```c
#define CHANNEL_HOP_INTERVAL_MS 500  // Change to desired ms
```

### Change Serial Baudrate

The default baudrate is 921600 for maximum throughput. To change it:

1. Edit `sdkconfig.defaults`: Change `CONFIG_ESP_CONSOLE_UART_BAUDRATE`
2. Edit Python scripts: Change `BAUDRATE` constant
3. Rebuild and flash

## Troubleshooting

### No packets appearing in Wireshark

1. **Check serial port**: Make sure you're using the correct serial port
   ```bash
   # Linux
   ls -l /dev/ttyUSB*
   ls -l /dev/ttyACM*
   
   # macOS
   ls -l /dev/tty.*
   
   # Windows
   # Check Device Manager > Ports (COM & LPT)
   ```

2. **Check permissions** (Linux/macOS):
   ```bash
   sudo usermod -a -G dialout $USER
   # Log out and log back in
   ```

3. **Verify ESP32 is running**:
   ```bash
   idf.py -p /dev/ttyUSB0 monitor
   # You should see log messages
   ```

### Serial port errors

- **Linux**: You may need to add your user to the `dialout` group
- **Windows**: Install the USB-to-UART driver from the manufacturer
- **macOS**: Install the Silicon Labs CP210x driver if needed

### Low packet capture rate

1. Increase WiFi buffers in `sdkconfig.defaults`
2. Reduce channel hopping interval
3. Try a higher baudrate if your USB-UART supports it

## How It Works

1. **Promiscuous Mode**: ESP32-S3's WiFi is set to promiscuous mode, allowing it to capture all WiFi packets on the current channel
2. **Packet Capture**: Each packet is captured with timing information
3. **Pcap Format**: Packets are formatted with pcap headers (including radiotap headers for radio information)
4. **Serial Streaming**: Formatted packets are sent over serial at 921600 baud
5. **Wireshark Display**: Python script pipes the data to Wireshark for real-time display and analysis

## Packet Types Captured

The sniffer captures all 802.11 frame types:
- Management frames (Beacon, Probe Request/Response, Authentication, etc.)
- Control frames (RTS, CTS, ACK, etc.)
- Data frames (including encrypted frames)

## Performance

- **Baudrate**: 921600 bps (~115 KB/s theoretical)
- **Buffer size**: 32 dynamic RX buffers
- **Channel hop rate**: 500ms per channel (configurable)
- **Capture rate**: Depends on WiFi traffic density

## Security Notice

This tool is intended for educational purposes and authorized network testing only. Ensure you have permission to monitor WiFi networks. Unauthorized packet sniffing may be illegal in your jurisdiction.

## License

This project is open source. Please check the repository for license information.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## References

- [ESP-IDF WiFi Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/network/esp_wifi.html)
- [Wireshark pcap format](https://wiki.wireshark.org/Development/LibpcapFileFormat)
- [IEEE 802.11 Standard](https://en.wikipedia.org/wiki/IEEE_802.11)