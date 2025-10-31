# WiFi Sniffer Examples

This directory contains example scripts for capturing and processing WiFi packets from the sniffer.

## Prerequisites

For Python examples:
```bash
pip install -r requirements.txt
```

## Examples

### 1. capture_packets.py

Python script to capture and display packets in real-time.

**Usage:**
```bash
# Linux/macOS
python3 capture_packets.py /dev/ttyACM0

# Windows
python3 capture_packets.py COM3
```

**Features:**
- Real-time packet display
- Packet statistics every 100 packets
- Graceful handling of Ctrl+C

### 2. save_to_file.sh

Bash script to save captured packets to a file.

**Usage:**
```bash
# With automatic filename
./save_to_file.sh /dev/ttyACM0

# With custom filename
./save_to_file.sh /dev/ttyACM0 my_capture.log
```

**Features:**
- Saves packets with timestamp in filename
- Displays output on screen while saving
- Creates log file in current directory

## Tips

### Finding the Serial Port

**Linux:**
```bash
ls /dev/ttyACM*
# or
dmesg | grep tty
```

**macOS:**
```bash
ls /dev/cu.usbmodem*
```

**Windows:**
Open Device Manager and look for "USB Serial Device" under "Ports (COM & LPT)"

### Packet Analysis

To analyze captured packets with Wireshark or other tools:

1. Save packets to file using `save_to_file.sh`
2. Parse the packet data (remove headers)
3. Convert to pcap format if needed

Example parsing:
```bash
grep "^\[CH:" packets.log | cut -d']' -f2- > raw_packets.bin
```

## Troubleshooting

### Permission Denied (Linux)

Add your user to the dialout group:
```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

### No Data Received

1. Verify USB connection is working
2. Check that the ESP32-S3 is programmed and running
3. Ensure you're using the correct serial port
4. Try resetting the ESP32-S3 board
