# ESP32-S3 Board 

<br/>



<br/>


* ESP32-S3 DevkitC1     
반드시 2개의 USB를 사용해서 진행  아래는 오직 TEST 용    
![](imgs/esp32-s3_devkit_00.png)

<br/>

* USB Ports   
   * Left USB : Serial Interface  (ESP-Flash/Monitor)           
   * Right USB : USB or JTAG      (Not used JTAG, USB-CDC)         

세부사항을 아래 

<br/>


## Firmware 

<br/>

* Python Capture   

USB-CDC ACM (즉 Serial Monitor 로)로 Packet을 받아 이를 저장하므로, USB 와 같이 Python 으로 Pcap저장        

<br/>


* Python Setup      
```
pip install -r .\requirements.txt
```


* Python Package Backup 
```
pip freeze > requirements.txt
```

<br/>

## Hardware

<br/>

* ESP32-S3 DevkitC-1 Manual          
   * [ESP32-S3 DevkitC-1 Manual](https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)   
   ![](imgs/esp32-s3_devkit_03.png)

<br/>

* ESP32-S3 DevkitC-1 Schemetic      
   * [ESP32-S3 DevkitC-1](https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DevKitC-1_V1.1_20221130.pdf)
   * ESP32-S3 DevkitC1 의 USB Port 2개 구성 
      * USB To UART(USB To Serial, CP2102N ) ---> 세부내용은 회로도 확인            
      * ESP USB (아래 각 포트확인)         
      ![](imgs/esp32-s3_devkit_01.png)
      ![](imgs/esp32-s3_devkit_02.png)

<br/>


## USB Ports 

<br/>

* USB Ports     
   1. COM A (CP2102N): 플래시 / 모니터(로그)
      * 상위 USB To Serial 로 ESP32-S3 Monitor와 명령어 전달        
   2. COM B (TinyUSB CDC)
      * Wi-Fi 스니퍼 캡처 스트림 (Python 파서로 pcap 저장 → Wireshark)        

<br/>

* USB-CDC ACM 을 이용하여 사용예정 
   * 2번째는 TinyUSB 기반의 CDC를 이용하여 사용    
   * [TinyUSB 이해](https://docs.espressif.com/projects/esp-iot-solution/en/latest/usb/usb_overview/tinyusb_guide.html)          

<br/>

