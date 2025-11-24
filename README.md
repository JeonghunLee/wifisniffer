#  WiFi Sniffer with USB Gadget


A WiFi packet sniffer for ESP32-S3 based on ESP-IDF 5.4 that captures WiFi packets in promiscuous mode and streams them via USB CDC-ACM interface.

## Features

- **WiFi Packet Sniffing**: Captures WiFi management, control, and data frames in promiscuous mode
- **USB Gadget Interface**: Outputs captured packets via USB CDC-ACM (appears as a serial port)
- **Channel Hopping**: Automatically cycles through WiFi channels 1-13   ---> 좀 후에 HT20/40 확인 후    
- **ESP-IDF 5.4**: Built with the latest ESP-IDF framework
- **ESP32-S3 Support**: Optimized for ESP32-S3 with native USB support


## Boards

<br/>

- [ESP32-S3 DevkitC1 WIFI Sniffer](docs/board.md)      

- Wireshark 
    - [Wireshark WIFI Sniffer](docs/wireshark_0.md)      

<br/>


## Branchs 

<br/>

- [Main Branch](https://github.com/JeonghunLee/wifisniffer/tree/main)

<br/>

* Copilot TEST  
    [Copilot test1](https://github.com/JeonghunLee/wifisniffer/tree/copilot/create-wifi-sniffer-esp32-s3)               
    [Copilot test2](https://github.com/JeonghunLee/wifisniffer/tree/copilot/create-wifi-sniffer-usb-gadget)                       

<br/>

