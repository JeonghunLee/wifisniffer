#pragma once
#include <stdint.h>
#include "esp_err.h"



#define MAX_PACKET_SIZE    1536
#define RING_SLOTS_PSRAM   3072
#define RING_SLOTS_INTERNAL     256


#define SYNC_MAGIC             0xDEADBEEF
#define PACKET_HEADER_MAGIC    0xA5A5A5A5

/* on-wire prefix = 16B (packed) */
typedef struct __attribute__((packed)) {
  uint32_t magic;     /* PACKET_HEADER_MAGIC */
  uint32_t ts_sec;    /* epoch seconds */
  uint32_t ts_usec;   /* microseconds */
  uint8_t  channel;   /* Wi-Fi channel */
  int8_t   rssi;      /* RSSI dBm */
  uint16_t len;       /* payload length (<= MAX_PACKET_SIZE) */
 }__attribute__((packed)) packet_prefix_t;



/* Initialization and start helpers */
void sniffer_init(void);

