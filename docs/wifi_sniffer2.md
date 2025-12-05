# WIFI Sniffer TEST-2   

<br/>



<br/>

* Terminal -> Run Task        
    * Capture USB CDC (Not CP210x)-Simpel            

<br/>

.pcap로 저장하며, 기본으로 output/capture_날짜로 저장 확인  
이 파일 기반으로 wireshark로 분석   

<br/>
 
* python .\read_packets.py  

```
python .\read_packets.py 

Available serial ports:
  [1] COM4: Silicon Labs CP210x USB to UART Bridge(COM4)   // for ESP-IDF Monitor 
  [2] COM11: USB 직렬 장치(COM11)                           // for WIFI Sniffer 

 *  Executing task: python read_packets.py  


Available serial ports:
  [1] COM4: Silicon Labs CP210x USB to UART Bridge(COM4)
  [2] COM3: USB 직렬 장치(COM3)                 

Select port number (or 'q' to quit): 2
✓ Connected to COM3 @ 115200 baud
✓ Saving to: ./output/capture_20251126_122756.pcap

======================================================================
Capturing WiFi packets... Press Ctrl+C to stop
======================================================================
[    1] 09:02:45.201348 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[    2] 09:02:45.266269 | Ch: 1 | RSSI:-89dBm | Len:  56B | MGMT
[    3] 09:02:45.267964 | Ch: 1 | RSSI:-89dBm | Len:  56B | MGMT
[    4] 09:02:45.303749 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[    5] 09:02:48.068576 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[    6] 09:02:48.075694 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[    7] 09:02:48.076537 | Ch: 1 | RSSI:-92dBm | Len:  28B | DATA
[    8] 09:02:48.084274 | Ch: 1 | RSSI:-90dBm | Len: 106B | MGMT
[    9] 09:03:06.994873 | Ch: 1 | RSSI:-91dBm | Len: 178B | MGMT
[   10] 09:03:07.012824 | Ch: 1 | RSSI:-41dBm | Len: 271B | MGMT
[   11] 09:03:07.042714 | Ch: 1 | RSSI:-90dBm | Len:  86B | MGMT
[   12] 09:03:07.097274 | Ch: 1 | RSSI:-89dBm | Len: 178B | MGMT
[   13] 09:03:07.115240 | Ch: 1 | RSSI:-41dBm | Len: 271B | MGMT
[   14] 09:03:07.121835 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   15] 09:03:07.122698 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   16] 09:03:07.125581 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   17] 09:03:07.187627 | Ch: 1 | RSSI:-92dBm | Len:  28B | DATA
[   18] 09:03:07.188479 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   19] 09:03:07.199684 | Ch: 1 | RSSI:-89dBm | Len: 178B | MGMT
[   20] 09:03:07.217641 | Ch: 1 | RSSI:-42dBm | Len: 271B | MGMT
[   21] 09:03:07.221536 | Ch: 1 | RSSI:-46dBm | Len:  30B | DATA
[   22] 09:03:07.224301 | Ch: 1 | RSSI:-92dBm | Len:  28B | DATA
[   23] 09:03:07.289597 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   24] 09:03:07.293295 | Ch: 1 | RSSI:-92dBm | Len:  56B | MGMT
[   25] 09:03:07.302076 | Ch: 1 | RSSI:-90dBm | Len: 178B | MGMT
[   26] 09:03:07.320052 | Ch: 1 | RSSI:-42dBm | Len: 271B | MGMT
[   27] 09:03:07.326702 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[   28] 09:03:07.327919 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   29] 09:03:07.338910 | Ch: 1 | RSSI:-92dBm | Len:  95B | MGMT
[   30] 09:03:07.339881 | Ch: 1 | RSSI:-93dBm | Len:  86B | MGMT
[   31] 09:03:07.375830 | Ch: 1 | RSSI:-92dBm | Len:  95B | MGMT
[   32] 09:03:07.376881 | Ch: 1 | RSSI:-92dBm | Len:  86B | MGMT
[   33] 09:03:07.404491 | Ch: 1 | RSSI:-90dBm | Len: 178B | MGMT
[   34] 09:03:07.422419 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   35] 09:03:07.506879 | Ch: 1 | RSSI:-88dBm | Len: 178B | MGMT
[   36] 09:03:07.524829 | Ch: 1 | RSSI:-42dBm | Len: 269B | MGMT
[   37] 09:03:07.532223 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   38] 09:03:07.627240 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   39] 09:03:07.633834 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   40] 09:03:07.711693 | Ch: 1 | RSSI:-90dBm | Len: 178B | MGMT
[   41] 09:03:07.729641 | Ch: 1 | RSSI:-41dBm | Len: 269B | MGMT
[   42] 09:03:07.736287 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   43] 09:03:07.814080 | Ch: 1 | RSSI:-90dBm | Len: 178B | MGMT
[   44] 09:03:07.832024 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   45] 09:03:07.839033 | Ch: 1 | RSSI:-97dBm | Len:  28B | DATA
[   46] 09:03:07.842777 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   47] 09:03:07.901553 | Ch: 1 | RSSI:-93dBm | Len:  64B | MGMT
[   48] 09:03:07.902373 | Ch: 1 | RSSI:-91dBm | Len:  64B | MGMT
[   49] 09:03:07.916491 | Ch: 1 | RSSI:-89dBm | Len: 178B | MGMT
[   50] 09:03:07.934444 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   51] 09:03:07.941101 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   52] 09:03:07.941953 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   53] 09:03:07.944788 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   54] 09:03:08.018882 | Ch: 1 | RSSI:-88dBm | Len: 178B | MGMT
[   55] 09:03:08.036868 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   56] 09:03:08.043579 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[   57] 09:03:08.139238 | Ch: 1 | RSSI:-42dBm | Len: 269B | MGMT
[   58] 09:03:08.145847 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   59] 09:03:08.223693 | Ch: 1 | RSSI:-88dBm | Len: 178B | MGMT
[   60] 09:03:08.241630 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   61] 09:03:08.248246 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[   62] 09:03:08.296118 | Ch: 1 | RSSI:-87dBm | Len: 106B | MGMT
[   63] 09:03:08.297068 | Ch: 1 | RSSI:-88dBm | Len:  86B | MGMT
[   64] 09:03:08.327252 | Ch: 1 | RSSI:-90dBm | Len: 178B | MGMT
[   65] 09:03:08.332762 | Ch: 1 | RSSI:-87dBm | Len: 106B | MGMT
[   66] 09:03:08.333735 | Ch: 1 | RSSI:-88dBm | Len:  86B | MGMT
[   67] 09:03:08.344024 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   68] 09:03:08.350639 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   69] 09:03:08.351492 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   70] 09:03:08.356309 | Ch: 1 | RSSI:-90dBm | Len:  50B | MGMT
[   71] 09:03:08.357035 | Ch: 1 | RSSI:-89dBm | Len:  50B | MGMT
[   72] 09:03:08.373033 | Ch: 1 | RSSI:-89dBm | Len: 106B | MGMT
[   73] 09:03:08.413869 | Ch: 1 | RSSI:-88dBm | Len: 106B | MGMT
[   74] 09:03:08.446450 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   75] 09:03:08.453049 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   76] 09:03:08.468161 | Ch: 1 | RSSI:-89dBm | Len:  50B | MGMT
[   77] 09:03:08.517804 | Ch: 1 | RSSI:-94dBm | Len:  28B | DATA
[   78] 09:03:08.518406 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   79] 09:03:08.548840 | Ch: 1 | RSSI:-42dBm | Len: 269B | MGMT
[   80] 09:03:08.555759 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[   81] 09:03:08.558557 | Ch: 1 | RSSI:-92dBm | Len:  28B | DATA
[   82] 09:03:08.651244 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   83] 09:03:08.657857 | Ch: 1 | RSSI:-95dBm | Len:  28B | DATA
[   84] 09:03:08.658718 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   85] 09:03:08.661609 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   86] 09:03:08.733816 | Ch: 1 | RSSI:-93dBm | Len:  56B | MGMT
[   87] 09:03:08.734553 | Ch: 1 | RSSI:-93dBm | Len:  56B | MGMT
[   88] 09:03:08.753640 | Ch: 1 | RSSI:-42dBm | Len: 269B | MGMT
[   89] 09:03:08.761082 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   90] 09:03:08.761934 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   91] 09:03:08.764188 | Ch: 1 | RSSI:-91dBm | Len:  28B | DATA
[   92] 09:03:08.856036 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   93] 09:03:08.863066 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   94] 09:03:08.863918 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   95] 09:03:08.865736 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   96] 09:03:08.958448 | Ch: 1 | RSSI:-43dBm | Len: 269B | MGMT
[   97] 09:03:08.966709 | Ch: 1 | RSSI:-90dBm | Len:  28B | DATA
[   98] 09:03:08.968746 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA
[   99] 09:03:09.029862 | Ch: 1 | RSSI:-93dBm | Len:  28B | DATA

======================================================================
✓ Capture stopped
======================================================================
  Total packets: 99
  Errors:        2361
  Saved to:      ./output/capture_20251126_122756.pcap

💡 Open with Wireshark: wireshark ./output/capture_20251126_122756.pcap

✓ Closed
```