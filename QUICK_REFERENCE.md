# Quick Reference Card

## Essential Commands

### Building and Flashing

```bash
# First time setup
. $HOME/esp/esp-idf/export.sh
idf.py set-target esp32s3

# Build
idf.py build

# Flash
idf.py -p /dev/ttyUSB0 flash

# Monitor (debug)
idf.py -p /dev/ttyUSB0 monitor
```

### Capturing Packets

```bash
# Real-time Wireshark
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0

# Save to file
python3 tools/capture_to_file.py /dev/ttyUSB0 capture.pcap

# With statistics
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -s -o capture.pcap
```

## Configuration Presets

### Monitor Single Channel (Channel 6)

Edit `main/wifisniffer.c`:
```c
#define CHANNEL_HOP_ENABLED 0
static uint8_t current_channel = 6;
```

### Faster Channel Hopping

Edit `main/wifisniffer.c`:
```c
#define CHANNEL_HOP_INTERVAL_MS 250  // 250ms instead of 500ms
```

### Monitor Only 2.4GHz Non-Overlapping Channels

Edit `main/wifisniffer.c`:
```c
static void channel_hop_task(void *pvParameter) {
    uint8_t channels[] = {1, 6, 11};
    uint8_t idx = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(CHANNEL_HOP_INTERVAL_MS));
        idx = (idx + 1) % 3;
        current_channel = channels[idx];
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
    }
}
```

## Common Wireshark Filters

```
Beacon frames:          wlan.fc.type_subtype == 0x08
Probe requests:         wlan.fc.type_subtype == 0x04
Data frames:            wlan.fc.type == 2
Specific SSID:          wlan.ssid == "NetworkName"
Specific MAC:           wlan.addr == aa:bb:cc:dd:ee:ff
Deauth frames:          wlan.fc.type_subtype == 0x0c
Management frames:      wlan.fc.type == 0
Control frames:         wlan.fc.type == 1
```

## Troubleshooting

### Permission Denied (Linux)
```bash
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### Find Serial Port
```bash
# Linux
ls /dev/ttyUSB* /dev/ttyACM*

# macOS
ls /dev/tty.*

# Windows - Check Device Manager
```

### ESP32 Not Responding
```bash
# Press and hold BOOT button, press RESET, release BOOT
idf.py -p /dev/ttyUSB0 flash
```

### Clear Build and Rebuild
```bash
idf.py fullclean
idf.py build
```

## File Locations

| File | Purpose |
|------|---------|
| `main/wifisniffer.c` | Main firmware code |
| `sdkconfig.defaults` | ESP32 configuration |
| `tools/sniffer_to_wireshark.py` | Live capture script |
| `tools/capture_to_file.py` | File capture script |
| `tools/advanced_sniffer.py` | Advanced capture with stats |

## Serial Ports by Platform

| Platform | Typical Port |
|----------|--------------|
| Linux | `/dev/ttyUSB0` or `/dev/ttyACM0` |
| macOS | `/dev/tty.usbserial-*` |
| Windows | `COM3`, `COM4`, etc. |

## Performance Tips

1. Use 921600 baud (default) for best throughput
2. Disable channel hopping for focused capture
3. Use short, high-quality USB cables
4. Close other serial applications
5. Increase WiFi buffer sizes in `sdkconfig.defaults` if needed

## WiFi Channels (2.4GHz)

| Channel | Frequency | Region |
|---------|-----------|--------|
| 1-11 | 2412-2462 MHz | Worldwide |
| 12-13 | 2467-2472 MHz | Everywhere except US |
| 14 | 2484 MHz | Japan only |

## Useful Links

- Documentation: See README.md and SETUP_GUIDE.md
- Examples: See EXAMPLES.md
- ESP-IDF: https://docs.espressif.com/projects/esp-idf/
- Wireshark: https://www.wireshark.org/docs/
