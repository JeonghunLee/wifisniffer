# ESP32-S3 WiFi Sniffer Setup Guide

This guide will walk you through setting up the WiFi sniffer on your ESP32-S3 DevKit-C1.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Installing ESP-IDF](#installing-esp-idf)
3. [Building the Project](#building-the-project)
4. [Flashing the ESP32-S3](#flashing-the-esp32-s3)
5. [Setting Up Python Environment](#setting-up-python-environment)
6. [Capturing Packets](#capturing-packets)
7. [Troubleshooting](#troubleshooting)

## Prerequisites

### Hardware
- ESP32-S3 DevKit-C1 board
- USB-C cable (for power and data)
- Computer (Windows, macOS, or Linux)

### Software
- Git
- Python 3.6 or later
- ESP-IDF v4.4 or later (v5.x recommended)
- Wireshark (for packet analysis)

## Installing ESP-IDF

### Linux and macOS

1. **Install prerequisites**:

   ```bash
   # Ubuntu/Debian
   sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   
   # macOS (using Homebrew)
   brew install cmake ninja dfu-util
   ```

2. **Clone ESP-IDF**:

   ```bash
   mkdir -p ~/esp
   cd ~/esp
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   git checkout v5.1.2  # Or latest stable version
   ```

3. **Install ESP-IDF**:

   ```bash
   cd ~/esp/esp-idf
   ./install.sh esp32s3
   ```

4. **Set up environment** (do this every time you open a new terminal):

   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

   Add this to your `.bashrc` or `.zshrc` for automatic setup:
   ```bash
   alias get_idf='. $HOME/esp/esp-idf/export.sh'
   ```

### Windows

1. **Download ESP-IDF installer**:
   - Go to https://dl.espressif.com/dl/esp-idf/
   - Download the latest ESP-IDF Tools Installer for Windows

2. **Run the installer**:
   - Follow the installation wizard
   - Select "ESP32-S3" as the target
   - Complete the installation

3. **Open ESP-IDF Command Prompt**:
   - Use the "ESP-IDF Command Prompt" from the Start menu

## Building the Project

1. **Clone this repository** (if you haven't already):

   ```bash
   git clone https://github.com/JeonghunLee/wifisniffer.git
   cd wifisniffer
   ```

2. **Set up ESP-IDF environment**:

   ```bash
   # Linux/macOS
   . $HOME/esp/esp-idf/export.sh
   
   # Windows: Use ESP-IDF Command Prompt
   ```

3. **Set target to ESP32-S3**:

   ```bash
   idf.py set-target esp32s3
   ```

4. **Build the project**:

   ```bash
   idf.py build
   ```

   This will compile the firmware. The first build takes a few minutes.

## Flashing the ESP32-S3

1. **Connect your ESP32-S3** to your computer via USB-C

2. **Find the serial port**:

   ```bash
   # Linux
   ls /dev/ttyUSB* /dev/ttyACM*
   
   # macOS
   ls /dev/tty.*
   
   # Windows (check Device Manager or try)
   # COM3, COM4, etc.
   ```

3. **Flash the firmware**:

   ```bash
   # Replace /dev/ttyUSB0 with your port
   idf.py -p /dev/ttyUSB0 flash
   
   # Windows example
   idf.py -p COM3 flash
   ```

4. **Monitor the output** (optional):

   ```bash
   idf.py -p /dev/ttyUSB0 monitor
   ```

   Press `Ctrl+]` to exit the monitor.

## Setting Up Python Environment

1. **Install Python dependencies**:

   ```bash
   pip install -r requirements.txt
   ```

   Or manually:
   ```bash
   pip install pyserial
   ```

2. **Install Wireshark**:

   - **Linux**: `sudo apt-get install wireshark` or use your package manager
   - **macOS**: Download from https://www.wireshark.org/ or `brew install wireshark`
   - **Windows**: Download from https://www.wireshark.org/

3. **Set permissions** (Linux only):

   ```bash
   # Add your user to dialout group for serial access
   sudo usermod -a -G dialout $USER
   
   # Add your user to wireshark group for packet capture
   sudo usermod -a -G wireshark $USER
   
   # Log out and log back in for changes to take effect
   ```

## Capturing Packets

### Method 1: Real-time Capture with Wireshark

```bash
python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

This will:
1. Connect to the ESP32-S3
2. Automatically launch Wireshark
3. Start displaying packets in real-time

### Method 2: Capture to File

```bash
python3 tools/capture_to_file.py /dev/ttyUSB0 my_capture.pcap
```

Then open the file in Wireshark:
```bash
wireshark my_capture.pcap
```

### Method 3: Advanced Capture with Statistics

```bash
# Capture with statistics
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -s

# Capture to file with statistics
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -o capture.pcap -s

# Capture exactly 1000 packets
python3 tools/advanced_sniffer.py /dev/ttyUSB0 -o capture.pcap -c 1000
```

## Troubleshooting

### ESP32-S3 not detected

1. **Check USB cable**: Ensure you're using a data cable, not just a charging cable
2. **Install drivers**:
   - Windows: Install CH340/CP210x drivers if needed
   - Linux: Should work out of the box
   - macOS: May need Silicon Labs driver

3. **Check Device Manager** (Windows) or `lsusb` (Linux) to see if device is recognized

### Permission denied on serial port (Linux)

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Or use sudo temporarily
sudo python3 tools/sniffer_to_wireshark.py /dev/ttyUSB0
```

### Wireshark shows no packets

1. **Check serial connection**: Make sure ESP32 is running (check monitor output)
2. **Verify baudrate**: Ensure script uses 921600 baud (default)
3. **Restart ESP32**: Press reset button on the board
4. **Check WiFi activity**: Ensure there's WiFi traffic in range

### Build errors

1. **Clean and rebuild**:
   ```bash
   idf.py fullclean
   idf.py build
   ```

2. **Update ESP-IDF**:
   ```bash
   cd ~/esp/esp-idf
   git pull
   git submodule update --init --recursive
   ./install.sh esp32s3
   ```

### Low packet capture rate

1. **Increase buffer sizes** in `sdkconfig.defaults`
2. **Disable channel hopping** to focus on one channel
3. **Use higher baudrate** if your USB-UART converter supports it
4. **Check for USB connection issues**

## Next Steps

- **Analyze captured packets** in Wireshark
- **Filter specific traffic** using Wireshark display filters
- **Customize channel hopping** by modifying `main/wifisniffer.c`
- **Adjust capture settings** for your specific use case

## Additional Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- [Wireshark User Guide](https://www.wireshark.org/docs/wsug_html_chunked/)
- [IEEE 802.11 Frame Format](https://en.wikipedia.org/wiki/IEEE_802.11)
- [Pcap File Format](https://wiki.wireshark.org/Development/LibpcapFileFormat)

## Support

For issues or questions:
- Check the [README.md](README.md) for common solutions
- Review ESP-IDF documentation
- Open an issue on GitHub
