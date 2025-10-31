# WiFi Sniffer Architecture

## Overview

This project implements a WiFi packet sniffer for ESP32-S3 using ESP-IDF 5.4. It captures WiFi packets in promiscuous mode and streams them to a host computer via USB CDC-ACM interface.

## System Architecture

```
┌─────────────────────────────────────────────────────┐
│                    ESP32-S3                         │
│                                                     │
│  ┌──────────────┐         ┌──────────────┐        │
│  │              │         │              │        │
│  │  WiFi Radio  │────────▶│   WiFi       │        │
│  │ (Promiscuous │ Packets │   Sniffer    │        │
│  │    Mode)     │         │   Module     │        │
│  │              │         │              │        │
│  └──────────────┘         └──────┬───────┘        │
│                                   │                │
│                                   │ Callback       │
│                                   │                │
│                          ┌────────▼────────┐       │
│                          │                 │       │
│                          │  Main App       │       │
│                          │  - Packet       │       │
│                          │    Handler      │       │
│                          │  - Channel      │       │
│                          │    Hopping      │       │
│                          │                 │       │
│                          └────────┬────────┘       │
│                                   │                │
│                                   │ USB Data       │
│                                   │                │
│                          ┌────────▼────────┐       │
│                          │                 │       │
│                          │  USB Gadget     │       │
│                          │  (TinyUSB       │       │
│                          │   CDC-ACM)      │       │
│                          │                 │       │
│                          └────────┬────────┘       │
│                                   │                │
└───────────────────────────────────┼────────────────┘
                                    │
                                    │ USB Cable
                                    │
                          ┌─────────▼────────┐
                          │                  │
                          │  Host Computer   │
                          │  /dev/ttyACM0    │
                          │                  │
                          └──────────────────┘
```

## Component Details

### 1. WiFi Sniffer Module

**File:** `main/wifi_sniffer.c`, `main/wifi_sniffer.h`

**Responsibilities:**
- Initialize ESP32-S3 WiFi hardware in promiscuous mode
- Register packet capture callback
- Manage WiFi channel selection
- Provide control interface (start/stop/channel switch)

**Key APIs:**
- `wifi_sniffer_init()` - Initialize with callback
- `wifi_sniffer_start()` - Start packet capture
- `wifi_sniffer_stop()` - Stop packet capture
- `wifi_sniffer_set_channel()` - Switch WiFi channel
- `wifi_sniffer_get_channel()` - Get current channel

**Implementation Notes:**
- Uses ESP-IDF WiFi driver in NULL mode with promiscuous enabled
- Callback is set before enabling promiscuous mode to avoid missed packets
- Supports channels 1-13 (2.4 GHz)
- Captures management, control, and data frames

### 2. USB Gadget Module

**File:** `main/usb_gadget.c`, `main/usb_gadget.h`

**Responsibilities:**
- Initialize USB CDC-ACM interface using TinyUSB
- Provide simple send API for packet data
- Track USB connection state
- Handle USB events (DTR/RTS line states)

**Key APIs:**
- `usb_gadget_init()` - Initialize USB CDC-ACM
- `usb_gadget_send()` - Send data via USB
- `usb_gadget_is_connected()` - Check USB connection state

**Implementation Notes:**
- Uses TinyUSB library included in ESP-IDF 5.4
- Appears as standard USB serial port on host
- Connection state tracked via DTR/RTS signals
- Non-blocking send with queue

### 3. Main Application

**File:** `main/main.c`

**Responsibilities:**
- Application entry point
- Initialize both WiFi sniffer and USB gadget
- Implement packet handling logic
- Format packets with metadata
- Implement channel hopping task
- Provide status logging

**Key Features:**
- Configurable via Kconfig (channel, interval, max packet size)
- Automatic channel cycling through 1-13
- Packet metadata includes channel, sequence number, and length
- Statistics logging every 100 packets
- USB connection awareness

## Data Flow

1. **Packet Capture:**
   - WiFi hardware receives packet in promiscuous mode
   - ESP-IDF WiFi driver invokes registered callback
   - WiFi sniffer module receives packet in `wifi_sniffer_packet_handler()`

2. **Packet Processing:**
   - Callback forwards packet to application's `wifi_packet_handler()`
   - Application checks USB connection state
   - Application formats packet with metadata header
   - Application limits packet size to configured maximum

3. **USB Transmission:**
   - Application calls `usb_gadget_send()` with header
   - Application calls `usb_gadget_send()` with packet data
   - USB gadget queues data in TinyUSB CDC-ACM buffer
   - TinyUSB handles USB protocol and transmission to host

4. **Host Reception:**
   - Host OS receives USB CDC-ACM data
   - Data appears on serial port (e.g., /dev/ttyACM0)
   - User applications read from serial port

## Configuration

### Build-time Configuration (Kconfig)

Defined in `main/Kconfig.projbuild`:
- `CONFIG_WIFI_SNIFFER_DEFAULT_CHANNEL` - Starting channel (1-13)
- `CONFIG_WIFI_SNIFFER_CHANNEL_SWITCH_INTERVAL` - Hopping interval in ms
- `CONFIG_WIFI_SNIFFER_MAX_PACKET_SIZE` - Max bytes to transmit per packet

### Hardware Configuration (sdkconfig.defaults)

- ESP32-S3 as target
- TinyUSB enabled with CDC-ACM support
- WiFi buffers optimized for packet capture
- Custom partition table

## Memory Layout

### Partition Table (partitions.csv)

```
nvs:      24KB  - Non-volatile storage
phy_init:  4KB  - WiFi PHY initialization data
factory:   1MB  - Application binary
```

### RAM Usage

**Static Allocations:**
- WiFi RX buffers: 16 static + 32 dynamic
- WiFi TX buffers: 32 dynamic
- USB CDC-ACM buffer: 64 bytes unread buffer

**Dynamic Allocations:**
- Channel switching task: 2KB stack
- WiFi driver internal allocations
- TinyUSB internal buffers

## Performance Characteristics

### Throughput
- **Packet Capture Rate:** Up to ~1000 packets/second depending on WiFi traffic
- **USB Bandwidth:** CDC-ACM full-speed (12 Mbps theoretical)
- **Practical Throughput:** Sufficient for typical WiFi sniffing scenarios

### Latency
- **Packet to USB:** < 1ms typical
- **Channel Switch:** Configurable, default 5000ms
- **USB Polling:** Handled by TinyUSB stack

### Resource Usage
- **Flash:** ~500KB application code
- **RAM:** ~100KB runtime (WiFi + USB + application)
- **CPU:** ~20-30% at moderate packet rates

## Security Considerations

1. **Promiscuous Mode:** Can capture all WiFi packets in range, including those from other networks
2. **Data Privacy:** Captured data may contain sensitive information
3. **USB Access:** Host computer has direct access to captured packets
4. **No Encryption:** Data transmitted over USB is not encrypted
5. **Intended Use:** Educational and authorized network analysis only

## Extensibility

### Adding Features

**Packet Filtering:**
- Modify `wifi_packet_handler()` to filter by packet type, SSID, MAC address, etc.

**Data Processing:**
- Add analysis in packet handler before USB transmission
- Implement statistics collection (packet types, rates, etc.)

**Storage:**
- Add SD card support to log packets to file
- Implement circular buffer for local packet storage

**Multiple Channels:**
- Modify channel hopping to scan specific channel list
- Add dwell time configuration per channel

**Network Interface:**
- Replace USB with WiFi AP mode for wireless packet streaming
- Add web interface for configuration and monitoring

## Build System

### CMake Structure

```
wifisniffer/
├── CMakeLists.txt           # Top-level project
└── main/
    └── CMakeLists.txt       # Main component
```

### Dependencies

**ESP-IDF Components:**
- `esp_wifi` - WiFi driver and promiscuous mode
- `nvs_flash` - Non-volatile storage
- `esp_event` - Event loop
- `tinyusb` - USB device stack
- `freertos` - Real-time operating system

## Testing

### Unit Testing
- WiFi sniffer initialization
- Channel switching logic
- USB connection state tracking

### Integration Testing
- Packet capture and USB transmission
- Channel hopping functionality
- USB connection/disconnection handling

### System Testing
- End-to-end packet capture with host tools
- Performance under high packet rates
- Long-duration stability testing

## Future Enhancements

1. **PCAP Format Support:** Output packets in standard PCAP format
2. **Wireshark Integration:** Direct integration with Wireshark
3. **5GHz Support:** Add ESP32-S3 5GHz WiFi support
4. **BLE Sniffing:** Add Bluetooth LE packet capture
5. **Web Interface:** Add WiFi AP mode with web-based control
6. **Storage:** Add SD card support for packet logging
7. **Filtering:** Add runtime packet filtering configuration
8. **Statistics:** Add detailed packet statistics and analysis
