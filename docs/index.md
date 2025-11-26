# ESP32-S3 WIFI Sniffer 

<br/>

## Board 

<br/>

* H/W and F/W Information
   * [ESP32-S3 Board](board.md)

<br/>

## Wifi Control 

<br/>

WIFI 설정 및 관련 부분 Control 기능    

<br/>


* ESP32-S3 WIFI Control 
   * [WIFI-Control](command.md#wifi-command)        
   * [ESP-IDf Monitor](command.md)     

<br/>


## Wifi Sniffer 

<br/>

VS Code 기반으로 자동화   

<br/>

* VS Code 의 경우 자동실행 
   * Terminal -> Run Task    
      * **Capture USB CDC (Not CP210x)-Debug**    
      * **Capture USB CDC (Not CP210x)-Simpe**    

<br/>

* WIFI Sniffer TEST 방법 및 Capture방법           
   * [Wifi Sniffter only Display(Debug)](wifi_sniffer0.md) 
   * [Wifi Sniffer Capture(Debug)](wifi_sniffer1.md)
   * [Wifi Sniffer Capture(Simple)](wifi_sniffer2.md)   

<br/>


## Wireshark 

<br/>

* Wireshark 분석 방법              
   1. output의 *.pcap  Wireshark로 분석 
   2. 현재 STA/AP 설정을 하지 않았으므로, 기본적인 Beacon 과 PR만 확인가능   
   3. **WLAN(Ethernet2)이 아닌 WIFI Packet으로 분석가능**    
      * tcpdump로도 할 생각이였으나, WIFI Packet 분석가능하다    

<br/>

* Wireshark WIFI 분석 예제     
   * [Wireshark0](wireshark_0.md) 

<br/>
