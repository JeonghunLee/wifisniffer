#pragma once
#include <stdint.h>
#include "esp_err.h"


// Sniffer Maximum packet size and ring buffer sizes
/** @brief Maximum raw 802.11 payload length captured per frame. */
#define MAX_PACKET_SIZE         1536

// Ring buffer 슬롯 개수 (PSRAM 유무에 따라 다르게 설정)
/** @brief Number of ring slots when PSRAM is available. */
#define RING_SLOTS_PSRAM        3072  // 약 5MB (3072 * 4+16+1536 = 5,018,112B)
/** @brief Number of ring slots when limited to internal RAM. */
#define RING_SLOTS_INTERNAL     256


// USB CDC Sync Magic and Packet Magic
#define SYNC_MAGIC             0xDEADBEEF  // Stream and USB CDC Sync Magic
#define PACKET_HEADER_MAGIC    0xA5A5A5A5  // PACKET_PREFIX Magic number

/*

  Ring Buffer + USB_CDC Format: ring_slot_hdr_t
      - [SYNC_MAGIC(4B)][Ring Buffer(8)][PACKET_HEADER_MAGIC+PACKET_PREFIX(16B)][PAYLOAD] 

  USB_CDC Format: packet_prefix_t
      - [SYNC_MAGIC(4B)][PACKET_HEADER_MAGIC+PACKET_PREFIX(16B)][PAYLOAD]
*/ 

/* on-wire prefix = 16B (packed) */
/**
 * @brief Metadata prepended to each frame written to the USB CDC stream.
 */
typedef struct __attribute__((packed)) {
  uint32_t magic;     /* PACKET_HEADER_MAGIC */
  uint32_t ts_sec;    /* epoch seconds */
  uint32_t ts_usec;   /* microseconds */
  uint8_t  channel;   /* Wi-Fi channel */
  int8_t   rssi;      /* RSSI dBm */
  uint16_t len;       /* payload length (<= MAX_PACKET_SIZE) */
 }__attribute__((packed)) packet_prefix_t;



/* Initialization and start helpers */
/** @brief Set up ring buffer and streamer task. */
void sniffer_init(void);                 // ring buffer + streamer task
/** @brief Enable promiscuous capture filters and callback. */
void sniffer_enable_promiscuous(void);   // filter + callback + promiscuous on
/** @brief Reset ring buffer indices and statistics. */
void sniffer_ring_reset(void);           // ring buffer reset
/** @brief Dump capture/drop counters and current ring usage. */
void sniffer_print_stats(void);          // print captured/dropped counts (for cmd)