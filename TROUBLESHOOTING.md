# Troubleshooting Guide

This guide addresses common issues you might encounter when setting up and using the ESP32-S3 WiFi sniffer.

## Table of Contents

- [Build Issues](#build-issues)
- [Flashing Issues](#flashing-issues)
- [Serial Connection Issues](#serial-connection-issues)
- [Packet Capture Issues](#packet-capture-issues)
- [Wireshark Issues](#wireshark-issues)
- [Performance Issues](#performance-issues)

## Build Issues

### Error: "esp_wifi.h: No such file or directory"

**Cause**: ESP-IDF not properly installed or environment not set up.

**Solution**:
```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Verify ESP-IDF is available
echo $IDF_PATH

# Should print something like: /home/user/esp/esp-idf
```

### Error: "Target 'esp32' does not match"

**Cause**: Project not configured for ESP32-S3.

**Solution**:
```bash
# Clean and reconfigure
idf.py fullclean
idf.py set-target esp32s3
idf.py build
```

### Error: "CMake Error"

**Cause**: CMake cache issues or missing dependencies.

**Solution**:
```bash
# Remove build directory
rm -rf build sdkconfig sdkconfig.old

# Reconfigure and build
idf.py set-target esp32s3
idf.py build
```

### Warning: "Partition table not found"

**Cause**: Normal warning if custom partitions not yet generated.

**Solution**: This is usually not a problem. If builds fail, ensure `partitions.csv` exists in the project root.

## Flashing Issues

### Error: "Serial port not found" or "Permission denied"

**Linux/macOS**:
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Or use sudo temporarily
sudo idf.py -p /dev/ttyUSB0 flash

# Check if device exists
ls -l /dev/ttyUSB* /dev/ttyACM*

# Set temporary permissions
sudo chmod 666 /dev/ttyUSB0
```

**Windows**:
- Install USB-to-UART driver (CH340 or CP2102)
- Check Device Manager for COM port number
- Try different USB port

### Error: "Failed to connect to ESP32-S3"

**Solution 1 - Boot mode**:
```bash
# Put ESP32 in download mode:
# 1. Hold BOOT button
# 2. Press and release RESET button
# 3. Release BOOT button
# 4. Run flash command
idf.py -p /dev/ttyUSB0 flash
```

**Solution 2 - Check USB cable**:
- Use a data-capable USB cable (not charge-only)
- Try a different cable or USB port
- Use USB 2.0 port if USB 3.0 causes issues

**Solution 3 - Reset bootloader**:
```bash
# Erase flash and re-flash
idf.py -p /dev/ttyUSB0 erase-flash
idf.py -p /dev/ttyUSB0 flash
```

### Error: "Wrong boot mode detected"

**Cause**: ESP32 is in the wrong boot mode.

**Solution**:
```bash
# Force bootloader mode with esptool
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size detect 0x0 build/bootloader/bootloader.bin 0x10000 build/wifisniffer.bin 0x8000 build/partition_table/partition-table.bin
```

## Serial Connection Issues

### Python Error: "No module named 'serial'"

**Solution**:
```bash
pip install pyserial

# Or with pip3
pip3 install pyserial
```

### Error: "Could not open port /dev/ttyUSB0"

**Causes and Solutions**:

1. **Port in use by another application**:
```bash
# Find what's using the port
lsof /dev/ttyUSB0

# Kill the process or close the application
```

2. **Wrong port name**:
```bash
# List available ports
python3 -c "import serial.tools.list_ports; print([p.device for p in serial.tools.list_ports.comports()])"
```

3. **Device disconnected**:
- Check USB connection
- Try unplugging and replugging
- Check `dmesg` (Linux) for USB events

### Serial Output Shows Garbage Characters

**Cause**: Baudrate mismatch.

**Solution**:
```bash
# Verify baudrate in sdkconfig.defaults
grep BAUDRATE sdkconfig.defaults

# Should show: CONFIG_ESP_CONSOLE_UART_BAUDRATE=921600

# Verify Python script uses matching baudrate
grep BAUDRATE tools/*.py
```

## Packet Capture Issues

### No Packets Appearing in Wireshark

**Solution 1 - Verify ESP32 is running**:
```bash
# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor

# Look for these messages:
# "WiFi Sniffer started on channel X"
# "Pcap header written, starting packet capture..."
```

**Solution 2 - Check for WiFi activity**:
- Ensure there are WiFi devices nearby
- Try browsing internet on your phone/laptop
- Look for WiFi networks with a WiFi analyzer app

**Solution 3 - Verify pcap header**:
```bash
# Capture to file
python3 tools/capture_to_file.py /dev/ttyUSB0 test.pcap
# Wait 10 seconds, press Ctrl+C

# Check file has data
ls -lh test.pcap

# Verify pcap header
xxd test.pcap | head -3
# Should start with: d4 c3 b2 a1 (magic number in little-endian)
```

**Solution 4 - Reset device**:
```bash
# Re-flash firmware
idf.py -p /dev/ttyUSB0 flash

# Or press RESET button on ESP32
```

### Packets Captured But Appear Corrupted

**Cause**: Serial data corruption or baudrate issues.

**Solution**:
```bash
# Try lower baudrate
# Edit sdkconfig.defaults:
CONFIG_ESP_CONSOLE_UART_BAUDRATE=460800

# Edit Python scripts (change BAUDRATE = 460800)

# Rebuild and reflash
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

### Capture Misses Many Packets

**Causes and Solutions**:

1. **Serial buffer overflow**:
```bash
# Increase ESP32 WiFi buffers
# Edit sdkconfig.defaults:
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=64
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=64

# Rebuild
idf.py build flash
```

2. **USB bandwidth limitation**:
- Use USB 2.0 instead of USB 1.1
- Use shorter, higher-quality USB cable
- Close other applications using USB bandwidth

3. **Channel hopping too fast**:
```c
// Edit main/wifisniffer.c
#define CHANNEL_HOP_INTERVAL_MS 1000  // Slower hopping
```

## Wireshark Issues

### Wireshark Won't Start

**Linux**:
```bash
# Install Wireshark
sudo apt-get install wireshark

# Add user to wireshark group
sudo usermod -a -G wireshark $USER

# Log out and back in
```

**macOS**:
```bash
# Install via Homebrew
brew install wireshark

# Or download from https://www.wireshark.org
```

**Windows**:
- Download from https://www.wireshark.org
- Run installer as Administrator
- Install WinPcap/Npcap when prompted

### Wireshark Shows "Unknown Protocol"

**Cause**: Incorrect link-layer type.

**Solution**: This should not happen with proper pcap headers. If it does:
```bash
# Check pcap header in captured file
tshark -r capture.pcap -T fields -e frame.protocols | head -1

# Should show: radiotap:wlan or similar
```

### Wireshark Performance is Slow

**Solutions**:
1. Disable unnecessary protocol dissectors
2. Use display filters to focus on specific traffic
3. Increase Wireshark memory limit
4. Capture to file instead of live capture

## Performance Issues

### Low Packet Capture Rate

**Solution 1 - Optimize buffer sizes**:
```bash
# Edit sdkconfig.defaults
CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM=16
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=64
CONFIG_ESP_WIFI_AMPDU_RX_ENABLED=y
CONFIG_ESP_WIFI_AMPDU_TX_ENABLED=y
```

**Solution 2 - Disable channel hopping**:
```c
// Edit main/wifisniffer.c
#define CHANNEL_HOP_ENABLED 0
```

**Solution 3 - Increase baudrate** (if hardware supports):
```bash
# Edit sdkconfig.defaults
CONFIG_ESP_CONSOLE_UART_BAUDRATE=1500000

# Update Python scripts accordingly
```

### High CPU Usage on PC

**Solutions**:
1. Capture to file instead of live Wireshark
2. Use display filters to reduce visible packets
3. Disable unnecessary protocol dissectors
4. Upgrade Wireshark to latest version

### ESP32 Crashes or Reboots

**Solution 1 - Check power supply**:
- Use powered USB hub if needed
- Try different USB port
- Check USB cable quality

**Solution 2 - Monitor for errors**:
```bash
idf.py -p /dev/ttyUSB0 monitor

# Look for:
# - Stack overflow warnings
# - Panic messages
# - Memory allocation failures
```

**Solution 3 - Increase stack sizes**:
```c
// Edit main/wifisniffer.c
xTaskCreate(channel_hop_task, "channel_hop", 4096, NULL, 5, NULL);
// Increased from 2048 to 4096
```

## Getting More Help

If your issue isn't covered here:

1. **Check logs**:
```bash
idf.py -p /dev/ttyUSB0 monitor
```

2. **Enable verbose logging**:
```c
// Edit main/wifisniffer.c
// Change ESP_LOGI to ESP_LOGD for debug messages
```

3. **Test with minimal setup**:
```bash
# Flash and monitor without Python scripts
idf.py -p /dev/ttyUSB0 flash monitor
```

4. **Check ESP-IDF version**:
```bash
cd $IDF_PATH
git describe --tags
```

5. **Report issues**: Open an issue on GitHub with:
   - ESP-IDF version
   - Operating system
   - Complete error messages
   - Steps to reproduce

## Common Error Messages Reference

| Error Message | Likely Cause | Quick Fix |
|--------------|--------------|-----------|
| "Permission denied" | Serial port permissions | Add user to dialout group |
| "Port not found" | Wrong port or disconnected | Check USB connection |
| "Failed to connect" | ESP32 not in boot mode | Hold BOOT, press RESET |
| "CMake Error" | Build cache corrupted | Run `idf.py fullclean` |
| "No module named serial" | pyserial not installed | `pip install pyserial` |
| "Invalid pcap magic" | Baudrate mismatch | Check baudrate settings |
| Garbage in Wireshark | Serial corruption | Lower baudrate, better cable |
| No packets | WiFi not initialized | Check monitor output |

## Diagnostic Script

Run this to collect diagnostic information:

```bash
#!/bin/bash
echo "=== System Information ==="
uname -a
python3 --version

echo -e "\n=== ESP-IDF Version ==="
if [ -n "$IDF_PATH" ]; then
    cd $IDF_PATH && git describe --tags
else
    echo "ESP-IDF not found"
fi

echo -e "\n=== Serial Ports ==="
ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "No devices found"

echo -e "\n=== Python Packages ==="
pip list | grep serial

echo -e "\n=== USB Devices ==="
lsusb | grep -i "CP210\|CH340\|FTDI"

echo -e "\n=== Build Status ==="
ls -lh build/*.bin 2>/dev/null || echo "No build artifacts"
```

Save as `diagnose.sh`, make executable with `chmod +x diagnose.sh`, and run with `./diagnose.sh`.
