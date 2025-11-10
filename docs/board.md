# ESP32-S3

<br/>

![](imgs/esp32-s3_devkit_00.png)

![](imgs/esp32-s3_devkit_01.png)

![](imgs/esp32-s3_devkit_02.png)

<br/>

## Hardware

<br/>

[ESP32-S3 DevkitC-1](https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)

[ESP32-S3 DevkitC-1](https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DevKitC-1_V1.1_20221130.pdf)

<br/>


## USB Ports 

<br/>


1. COM A (CP2102N): 플래시 / 모니터(로그)
2. COM B (TinyUSB CDC): Wi-Fi 스니퍼 캡처 스트림 (Python 파서로 pcap 저장 → Wireshark)

<br/>

2번째는 TinyUSB 기반의 CDC를 이용하여 사용 
    [TinyUSB](https://docs.espressif.com/projects/esp-iot-solution/en/latest/usb/usb_overview/tinyusb_guide.html)

<br/>

## Wifi Sniffer 

<br/>

* WIFI Sniffer TEST 방법   
   * [Wifi Sniffter only Display](wifi_sniffer0.md) 
   * [Wifi Sniffer Capture](wifi_sniffer1.md)

<br/>


## Wireshark 

<br/>

* Wireshark 분석 
현재 STA/AP 설정을 하지 않았으므로, 기본적인 Beacon 과 PR만 확인가능  
   * [Wireshark0](wireshark_0.md) 

<br/>
