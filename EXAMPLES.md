# WiFi Sniffer Usage Examples

This document provides practical examples of using the ESP32-S3 WiFi sniffer.

## Table of Contents

1. [Basic Usage](#basic-usage)
2. [Capturing Specific Channels](#capturing-specific-channels)
3. [Analyzing Traffic](#analyzing-traffic)
4. [Advanced Filtering](#advanced-filtering)
5. [Common Use Cases](#common-use-cases)

## Basic Usage

### Quick Start - Live Capture

The simplest way to start capturing WiFi packets:

```bash
# 1. Flash the firmware (one time)
idf.py -p /dev/ttyUSB0 flash

# 2. Start live capture with Wireshark
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

### Save Capture to File

To save packets for later analysis:

```bash
# Start capture
python3 tools/capture_to_file.py /dev/ttyUSB0 my_capture.pcap

# Let it run for a while, then press Ctrl+C

# Analyze in Wireshark
wireshark my_capture.pcap
```

## Capturing Specific Channels

### Monitor a Single Channel

If you want to focus on a specific channel (e.g., channel 6):

1. Edit `main/wifisniffer.c`:
```c
#define CHANNEL_HOP_ENABLED 0        // Disable channel hopping
static uint8_t current_channel = 6;  // Set to desired channel (1-13)
```

2. Rebuild and flash:
```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

3. Capture packets:
```bash
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

### Custom Channel Hopping Sequence

To hop between specific channels only:

1. Modify the `channel_hop_task` function in `main/wifisniffer.c`:
```c
static void channel_hop_task(void *pvParameter) {
    uint8_t channels[] = {1, 6, 11};  // Only hop between these channels
    uint8_t channel_index = 0;
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_HOP_INTERVAL_MS));
        
        channel_index = (channel_index + 1) % 3;
        current_channel = channels[channel_index];
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
        ESP_LOGI(TAG, "Hopped to channel %d", current_channel);
    }
}
```

## Analyzing Traffic

### Wireshark Display Filters

Once capturing in Wireshark, use these filters to find specific traffic:

#### Find Beacon Frames
```
wlan.fc.type_subtype == 0x08
```

#### Find Probe Requests
```
wlan.fc.type_subtype == 0x04
```

#### Find Data Frames
```
wlan.fc.type == 2
```

#### Find Specific SSID
```
wlan.ssid == "YourNetworkName"
```

#### Find Specific MAC Address
```
wlan.addr == aa:bb:cc:dd:ee:ff
```

#### Find Deauth Packets
```
wlan.fc.type_subtype == 0x0c
```

### Capture Statistics

Use the advanced sniffer to see real-time statistics:

```bash
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -s
```

Output example:
```
--- Packet Statistics ---
Total packets: 5432
Total bytes: 2345678
Duration: 60.0s
Packets/sec: 90.5
Bits/sec: 312427
-------------------------
```

## Advanced Filtering

### Capture Limited Number of Packets

Capture exactly 1000 packets then stop:

```bash
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -o capture.pcap -c 1000
```

### Time-Based Capture

Capture for a specific duration using timeout:

```bash
# Capture for 5 minutes
timeout 300 python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

## Common Use Cases

### 1. Network Survey

Discover all WiFi networks in range:

```bash
# Capture for 60 seconds
timeout 60 python3 tools/capture_to_file.py /dev/ttyUSB0 survey.pcap

# Open in Wireshark and use filter:
# wlan.fc.type_subtype == 0x08
```

Look at beacon frames to see:
- SSIDs
- Channel information
- Signal strength
- Supported rates
- Security settings

### 2. Client Detection

Find all WiFi clients in the area:

```bash
# Capture traffic
python3 tools/capture_to_file.py /dev/ttyUSB0 clients.pcap

# In Wireshark, use Statistics > WLAN Traffic
# Or filter: wlan.fc.type_subtype == 0x04
```

### 3. Channel Utilization Analysis

Monitor which channels are most busy:

```bash
# Capture with channel hopping enabled (default)
python3 tools/capture_to_file.py /dev/ttyUSB0 channels.pcap

# In Wireshark: Statistics > I/O Graph
# Add filter for each channel, e.g.: radiotap.channel.freq == 2412
```

### 4. Security Analysis

Detect potential security issues:

```bash
# Capture traffic
python3 tools/capture_to_file.py /dev/ttyUSB0 security.pcap
```

In Wireshark, look for:
- Deauth attacks: `wlan.fc.type_subtype == 0x0c`
- WEP networks: `wlan.wep.key`
- Unencrypted data: `wlan.fc.type == 2 && !wlan.fc.protected`

### 5. Troubleshooting WiFi Issues

Debug connectivity problems:

```bash
# Focus on your network's channel
# Edit wifisniffer.c to disable hopping and set specific channel
idf.py build flash

# Capture during connection issues
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

Look for:
- Excessive retransmissions
- Deauth/Disassociation frames
- EAPOL failures (WPA handshake issues)
- High error rates

### 6. Performance Testing

Measure real-world WiFi performance:

```bash
# Capture during speed test
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -o performance.pcap -s

# Observe packets/sec and throughput in statistics
```

### 7. IoT Device Analysis

Monitor IoT device traffic patterns:

```bash
# Set channel to your IoT network's channel
# Capture for extended period
python3 tools/capture_to_file.py /dev/ttyUSB0 iot_traffic.pcap

# In Wireshark, filter by device MAC:
# wlan.addr == xx:xx:xx:xx:xx:xx
```

## Tips and Tricks

### Reduce Capture File Size

For long-term captures, use tshark to save only specific packets:

```bash
# Capture from ESP32
python3 tools/capture_to_file.py /dev/ttyUSB0 full_capture.pcap

# Filter and save subset
tshark -r full_capture.pcap -w filtered.pcap "wlan.fc.type_subtype == 0x08"
```

### Automated Capture Rotation

Capture with time-based rotation:

```bash
# Capture 5-minute segments
while true; do
    timestamp=$(date +%Y%m%d_%H%M%S)
    timeout 300 python3 tools/capture_to_file.py /dev/ttyUSB0 "capture_${timestamp}.pcap"
    sleep 1
done
```

### Remote Capture via SSH

Capture on remote machine and analyze locally:

```bash
# On remote machine with ESP32
ssh user@remote "python3 /path/to/sniffer_to_wireshark.py /dev/ttyUSB0" | wireshark -k -i -
```

### Combining with Other Tools

Use with tcpdump or other tools:

```bash
# Capture to pipe and process with tcpdump
python3 tools/capture_to_file.py /dev/ttyUSB0 /dev/stdout | tcpdump -r - -n
```

## Troubleshooting Examples

### No Packets Appearing

1. Check if ESP32 is working:
```bash
idf.py -p /dev/ttyUSB0 monitor
# You should see "Pcap header written, starting packet capture..."
```

2. Verify WiFi activity with another device

3. Check serial port permissions:
```bash
ls -l /dev/ttyUSB0
sudo chmod 666 /dev/ttyUSB0  # Or add user to dialout group
```

### Garbled or Corrupt Packets

1. Verify baudrate matches:
   - Check `sdkconfig.defaults`: CONFIG_ESP_CONSOLE_UART_BAUDRATE=921600
   - Check Python script: BAUDRATE = 921600

2. Test USB cable quality (use shorter, high-quality cable)

3. Try lower baudrate if issues persist:
   ```c
   // In sdkconfig.defaults
   CONFIG_ESP_CONSOLE_UART_BAUDRATE=460800
   ```

## Further Reading

- [Wireshark Display Filters](https://wiki.wireshark.org/DisplayFilters)
- [802.11 Frame Types](https://en.wikipedia.org/wiki/IEEE_802.11#Frames)
- [WiFi Security Best Practices](https://www.wi-fi.org/beacon/the-beacon/wi-fi-security-best-practices)

## Contributing

Have a useful example? Please contribute by opening a pull request!
