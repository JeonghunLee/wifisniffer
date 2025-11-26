# ESP32-S3 Monitor 

<br/>

[Go Back Index](index.md#wifi-control)

<br/>


## ESP-IDF Monitor 

<br/>

* ESP-IDF Monitor 
```
I (27) boot: ESP-IDF v5.4.3-277-gaac5b37fa3 2nd stage bootloader
I (27) boot: compile time Nov 25 2025 17:30:02
I (27) boot: Multicore bootloader
I (28) boot: chip revision: v0.1
I (31) boot: efuse block revision: v1.2
I (35) boot.esp32s3: Boot SPI Speed : 80MHz
I (39) boot.esp32s3: SPI Mode       : DIO
I (42) boot.esp32s3: SPI Flash Size : 8MB
I (46) boot: Enabling RNG early entropy source...
I (51) boot: Partition Table:
I (53) boot: ## Label            Usage          Type ST Offset   Length
I (60) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (66) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (73) boot:  2 factory          factory app      00 00 00010000 00100000
I (79) boot: End of partition table
I (82) esp_image: segment 0: paddr=00010020 vaddr=3c090020 size=1b5c8h (112072) map
I (110) esp_image: segment 1: paddr=0002b5f0 vaddr=3fc9b300 size=04a28h ( 18984) load
I (114) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=8b69ch (571036) map
I (215) esp_image: segment 3: paddr=000bb6c4 vaddr=3fc9fd28 size=00988h (  2440) load
I (216) esp_image: segment 4: paddr=000bc054 vaddr=40374000 size=1729ch ( 94876) load
I (239) esp_image: segment 5: paddr=000d32f8 vaddr=50000000 size=0001ch (    28) load
I (249) boot: Loaded app from partition at offset 0x10000
I (249) boot: Disabling RNG early entropy source...
I (259) octal_psram: vendor id    : 0x0d (AP)
I (259) octal_psram: dev id       : 0x02 (generation 3)
I (259) octal_psram: density      : 0x03 (64 Mbit)
I (261) octal_psram: good-die     : 0x01 (Pass)
I (265) octal_psram: Latency      : 0x01 (Fixed)
I (270) octal_psram: VCC          : 0x01 (3V)
I (274) octal_psram: SRF          : 0x01 (Fast Refresh)
I (279) octal_psram: BurstType    : 0x01 (Hybrid Wrap)
I (284) octal_psram: BurstLen     : 0x01 (32 Byte)
I (288) octal_psram: Readlatency  : 0x02 (10 cycles@Fixed)
I (293) octal_psram: DriveStrength: 0x00 (1/1)
I (298) MSPI Timing: PSRAM timing tuning index: 5
I (302) esp_psram: Found 8MB PSRAM device
I (306) esp_psram: Speed: 80MHz
I (322) mmu_psram: Read only data copied and mapped to SPIRAM
I (383) mmu_psram: Instructions copied and mapped to SPIRAM
I (383) cpu_start: Multicore app
I (761) esp_psram: SPI SRAM memory test OK
I (769) cpu_start: Pro cpu start user code
I (769) cpu_start: cpu freq: 160000000 Hz
I (769) app_init: Application information:
I (769) app_init: Project name:     esp32s3_sniffer
I (774) app_init: App version:      v1.0.0-8-g41e6701-dirty
I (779) app_init: Compile time:     Nov 25 2025 17:29:28
I (784) app_init: ELF file SHA256:  5b4d5ab17...
I (788) app_init: ESP-IDF:          v5.4.3-277-gaac5b37fa3
I (794) efuse_init: Min chip rev:     v0.0
I (797) efuse_init: Max chip rev:     v0.99
I (801) efuse_init: Chip rev:         v0.1
I (805) heap_init: Initializing. RAM available for dynamic allocation:
I (812) heap_init: At 3FCA4D30 len 000449E0 (274 KiB): RAM
I (817) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (822) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (827) heap_init: At 600FE000 len 00001FE8 (7 KiB): RTCRAM
I (832) esp_psram: Adding pool of 7488K of PSRAM memory to heap allocator
I (839) esp_psram: Adding pool of 18K of PSRAM memory gap generated due to end address alignment of drom to the heap allocator
I (850) spi_flash: detected chip: generic
I (854) spi_flash: flash io: dio
I (857) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (863) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (870) main_task: Started on CPU0
I (883) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (883) main_task: Calling app_main()
I (892) MAIN: NVS initialized
W (892) tusb_desc: No Device descriptor provided, using default.
W (892) tusb_desc: No FullSpeed configuration descriptor provided, using default.
W (900) tusb_desc: No String descriptors provided, using default.
I (906) tusb_desc:
┌─────────────────────────────────┐
│  USB Device Descriptor Summary  │
├───────────────────┬─────────────┤
│bDeviceClass       │ 239         │
├───────────────────┼─────────────┤
│bDeviceSubClass    │ 2           │
├───────────────────┼─────────────┤
│bDeviceProtocol    │ 1           │
├───────────────────┼─────────────┤
│bMaxPacketSize0    │ 64          │
├───────────────────┼─────────────┤
│idVendor           │ 0x303a      │
├───────────────────┼─────────────┤
│idProduct          │ 0x4001      │
├───────────────────┼─────────────┤
│bcdDevice          │ 0x100       │
├───────────────────┼─────────────┤
│iManufacturer      │ 0x1         │
├───────────────────┼─────────────┤
│iProduct           │ 0x2         │
├───────────────────┼─────────────┤
│iSerialNumber      │ 0x3         │
├───────────────────┼─────────────┤
│bNumConfigurations │ 0x1         │
└───────────────────┴─────────────┘
I (1073) TinyUSB: TinyUSB Driver installed
I (1076) USB_CDC: USB CDC initialized
I (1080) pp: pp rom version: e7ae62f
I (1083) net80211: net80211 rom version: e7ae62f
I (1089) wifi:wifi driver task: 3fcb7288, prio:23, stack:6656, core=0
I (1099) wifi:wifi firmware version: 4ec8aba
I (1099) wifi:wifi certification version: v7.0
I (1102) wifi:config NVS flash: enabled
I (1105) wifi:config nano formatting: disabled
I (1110) wifi:Init data frame dynamic rx buffer num: 32
I (1115) wifi:Init static rx mgmt buffer num: 5
I (1119) wifi:Init management short buffer num: 32
I (1123) wifi:Init dynamic tx buffer num: 32
I (1127) wifi:Init static tx FG buffer num: 2
I (1131) wifi:Init static rx buffer size: 1600
I (1136) wifi:Init static rx buffer num: 10
I (1139) wifi:Init dynamic rx buffer num: 32
I (1144) wifi_init: rx ba win: 6
I (1146) wifi_init: accept mbox: 6
I (1149) wifi_init: tcpip mbox: 32
I (1153) wifi_init: udp mbox: 6
I (1155) wifi_init: tcp mbox: 6
I (1158) wifi_init: tcp tx win: 5760
I (1162) wifi_init: tcp rx win: 5760
I (1165) wifi_init: tcp mss: 1440
I (1168) wifi_init: WiFi IRAM OP enabled
I (1172) wifi_init: WiFi RX IRAM OP enabled
I (1176) phy_init: phy_version 701,f4f1da3a,Mar  3 2025,15:50:10
I (1214) wifi:mode : null
I (1214) SNFFER: Streamer task started
I (1214) SNFFER: USB CDC ready: NO, USB CDC sent: 0 packets
I (1215) SNFFER: Sniffer started (Ring+CDC, prefix+payload stored)
I (1220) wifi:ic_enable_sniffer
I (1223) SNFFER: Promiscuous mode enabled
I (1226) WIFI_MGR: wifi_mgr_init: mode=null
W (1281) SNFFER: USB CDC not ready, dropping packet
```


* ESP32-S3 확인 후 
 * [Python 으로 Sniffer Capture 진행](wifi_sniffer2.md) 

```
W (76927) SNFFER: USB CDC not ready, dropping packet
W (77027) SNFFER: USB CDC not ready, dropping packet
W (77127) SNFFER: USB CDC not ready, dropping packet
W (77227) SNFFER: USB CDC not ready, dropping packet
W (77327) SNFFER: USB CDC not ready, dropping packet
W (77427) SNFFER: USB CDC not ready, dropping packet
W (77527) SNFFER: USB CDC not ready, dropping packet
W (77627) SNFFER: USB CDC not ready, dropping packet
W (77727) SNFFER: USB CDC not ready, dropping packet
W (77827) SNFFER: USB CDC not ready, dropping packet
W (77927) SNFFER: USB CDC not ready, dropping packet
W (78027) SNFFER: USB CDC not ready, dropping packet
W (78127) SNFFER: USB CDC not ready, dropping packet
I (78212) SNFFER: Captured: 2900, Dropped: 0  (WIFI Packet)
W (78227) SNFFER: USB CDC not ready, dropping packet
W (78327) SNFFER: USB CDC not ready, dropping packet
W (78427) SNFFER: USB CDC not ready, dropping packet
W (78527) SNFFER: USB CDC not ready, dropping packet
W (78627) SNFFER: USB CDC not ready, dropping packet
W (78727) SNFFER: USB CDC not ready, dropping packet
W (78827) SNFFER: USB CDC not ready, dropping packet
W (78928) SNFFER: USB CDC not ready, dropping packet
W (79028) SNFFER: USB CDC not ready, dropping packet
W (79128) SNFFER: USB CDC not ready, dropping packet
W (79228) SNFFER: USB CDC not ready, dropping packet
W (79328) SNFFER: USB CDC not ready, dropping packet
W (79428) SNFFER: USB CDC not ready, dropping packet
```

* ESP32-S3 의 Command 지원 
   * 각 WIFI MODE 설정 (ap/sta/ap+sta/null)
   * 각 CH 변경 
   * 각 상태 확인 
```
wifi>  wifI (271251) SNFFER: USB CDC ready: YES, USB CDC sent: 2796 packets
wifi>  wifiI (271350) SNFFER: USB CDC Sent: 2800 packets (Ring buffer size: 0)
wifi>  wifi
Usage:
  wifi mode <ap|sta|ap+sta|null>
  wifi sta ssid "<ssid>"
  wifi sta pw "<password>"
  wifi ap ssid "<ssid>"
  wifi ap pw "<password>"  (>=8 chars) or empty to open
  wifi ch <1-13>
  wifi ring reset
  wifi status
```





<br/>