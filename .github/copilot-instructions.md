# Copilot / AI agent instructions for wifisniffer

Short, actionable guidance so an AI coding agent is immediately productive with this ESP-IDF Wi‑Fi sniffer project.

## Big picture (what this repo is)
- Single ESP-IDF application targeting ESP32-S3 (see top-level `CMakeLists.txt` and `sdkconfig.defaults`).
- Purpose: capture Wi‑Fi frames in promiscuous mode and stream them over a USB CDC (TinyUSB) capture port to a host.
- Main implementation lives in `main/sniffer.c` (entrypoint: `app_main` → `sniffer_init_and_start`).
- Packet on-wire format: host reads 32-bit SYNC_MAGIC (0xDEADBEEF), 32-bit record length, then `packet_prefix_t` (16 bytes) + payload. Definition: `main/sniffer.h`.

## Key components & files (read first)
- `main/sniffer.c` — core: ring buffer, Wi‑Fi promiscuous callback, TinyUSB CDC streamer, FreeRTOS tasks.
- `main/sniffer.h` — packet header layout and build-time constants (MAX_PACKET_SIZE, DEFAULT_RING_PKTS_*).
- `sdkconfig.defaults` — defaults used by the build; note: target = `esp32s3`, TinyUSB enabled, console uses CP2102N (UART0). Adjust here before `menuconfig` if needed.
- `CMakeLists.txt` — small ESP-IDF project wrapper (uses `project.cmake`).

## Important runtime patterns & conventions (project-specific)
- Memory: prefers PSRAM when available. The ring buffer size is chosen at runtime: if PSRAM initialized, it uses `DEFAULT_RING_PKTS_PS` and allocates slots in SPIRAM; otherwise `DEFAULT_RING_PKTS_IR` in internal heap.
- Locking: ring buffer uses a FreeRTOS mutex (`xSemaphoreCreateMutex`) with non-blocking (or short wait) push/pop to keep interrupt/ISR safe behavior.
- Promiscuous capture: `esp_wifi_set_promiscuous(true)` + callback `wifi_promiscuous_cb` copies up to `MAX_PACKET_SIZE`.
- USB capture: TinyUSB CDC is used for a high-speed binary stream. Data is only written when host asserts DTR; check `sniffer_cdc_ready()`.
- Stream framing: streamer writes a sync word, record length (uint32), then header+payload; host-side parsers must sync on `SYNC_MAGIC`.

## Build / flash / debug (concrete commands for Windows PowerShell)
- Prereq: ESP-IDF toolchain and `idf.py` in PATH (standard ESP-IDF installation).
- Common commands (PowerShell):

```powershell
# configure (optional) and build
idf.py menuconfig        # edit sdkconfig; uses sdkconfig.defaults as baseline
idf.py build

# flash to device (auto-detect or specify COM port) and monitor
idf.py -p COM3 flash monitor  # replace COM3 with your serial port
# or set port via env before stepping
$env:ESPPORT='COM3'; idf.py -p $env:ESPPORT flash monitor
```

- Note: `sdkconfig.defaults` sets default baud for esptool to 921600 and flash size to 8MB; adjust if your board differs.

## Host integration notes (how host reads capture)
- Host reader must parse the TinyUSB CDC stream as: [SYNC_MAGIC:uint32][rec_len:uint32][packet_prefix_t][payload]. Use little-endian parsing.
- `packet_prefix_t` is defined in `main/sniffer.h` (packed 16 bytes: magic, ts_sec, ts_usec, channel, rssi, len).

## Quick examples (parsing hints)
- To find record boundaries: scan for 0xDEADBEEF (u32); next u32 is record length (len_header + payload).
- Do not rely on textual logs; the capture stream is binary.

## Edge cases & what to watch for
- TinyUSB: DTR must be asserted by host; otherwise no bytes are sent.
- Packet truncation: payload is capped at `MAX_PACKET_SIZE` (1536 by default).

## PR / change guidance (how to modify safely)
- When changing packet on-wire layout, update `main/sniffer.h` and the host parser together and bump the magic/version to avoid host mismatches.
- Memory changes: if you alter ring sizing constants, consider runtime PSRAM checks in `ring_init`.

## Where to look for more context
- `main/sniffer.c` — streaming logic and the best examples for FreeRTOS usage and USB writes.
- `sdkconfig.defaults` — target, console and TinyUSB configuration.

If anything above is unclear or you want additional examples (host parsing snippet in Python or a unit-test harness), just ask!
