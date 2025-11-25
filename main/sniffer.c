#include <string.h>
#include <sys/time.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "nvs_flash.h"           // ADD THIS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

// Project headers
#include "sniffer.h"
#include "ring_buf.h"
#include "usb_cdc.h" //usb_cdc_ready(), usb_cdc_write()

#define SLOT_SIZE (sizeof(packet_prefix_t) + MAX_PACKET_SIZE)

static const char *TAG = "SNFFER";

static ring_buf_t *g_rb = NULL;
static uint32_t captured_count = 0;
static uint32_t dropped_count = 0;

static bool have_psram(void){ 
  return heap_caps_get_free_size(MALLOC_CAP_SPIRAM) > 0; 
}


static void wifi_promiscuous_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
    if (type!=WIFI_PKT_MGMT && type!=WIFI_PKT_DATA &&
        type!=WIFI_PKT_CTRL && type!=WIFI_PKT_MISC) {
        return;
    }

    captured_count++;
    
    // Log every 100 packets
    if (captured_count % 100 == 0) {
      ESP_LOGI(TAG, "Captured: %lu, Dropped: %lu  (WIFI Packet)"
          ,captured_count
          ,dropped_count);
    }

    wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t *)buf;
    uint16_t len = p->rx_ctrl.sig_len;
    if (len > MAX_PACKET_SIZE) len = MAX_PACKET_SIZE;

    struct timeval tv; 
    gettimeofday(&tv, NULL);

    packet_prefix_t ph = {
      .magic   = PACKET_HEADER_MAGIC,
      .ts_sec  = (uint32_t)tv.tv_sec,
      .ts_usec = (uint32_t)tv.tv_usec,
      .channel = p->rx_ctrl.channel,
      .rssi    = p->rx_ctrl.rssi,
      .len     = len
    };

    uint8_t frame[sizeof(packet_prefix_t) + MAX_PACKET_SIZE];
    memcpy(frame, &ph, sizeof(ph));
    memcpy(frame + sizeof(ph), p->payload, len);

    ring_slot_hdr_t sh = {
        .magic = PACKET_HEADER_MAGIC,
        .len   = (uint16_t)(sizeof(ph) + len)
    };
    
    // Push data to ring buffer
    if (!ring_buf_push(g_rb, &sh, frame)) {
      dropped_count++;
      uint16_t rb_size = ring_buf_size(g_rb);
      ESP_LOGW(TAG, "Dropped packet (Ring buffer size: %u)", rb_size);
    }
}

/* ---------- Streamer task: Ring → CDC ---------- */
static void streamer_task(void *arg)
{
  (void)arg;
  ring_slot_hdr_t sh;
  
  uint8_t *ring_data = (uint8_t*)heap_caps_malloc(sizeof(packet_prefix_t) + MAX_PACKET_SIZE, MALLOC_CAP_8BIT);

  size_t max_tx_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(packet_prefix_t) + MAX_PACKET_SIZE;

  uint8_t *tx_buf = (uint8_t*)heap_caps_malloc(max_tx_size, MALLOC_CAP_8BIT);

  ESP_LOGI(TAG, "Streamer task started");
  
  uint32_t loop_count = 0;
  uint32_t packets_sent = 0;
  
  for (;;) {
    // Debug: Print status every 5 seconds
    if (loop_count % 2500 == 0) {  // 2500 * 2ms = 5 seconds
      bool cdc_ready = usb_cdc_ready();
      ESP_LOGI(TAG, "USB CDC ready: %s, USB CDC sent: %lu packets"
              ,cdc_ready ? "YES" : "NO"
              ,packets_sent);
    }
    
    loop_count++;
    
    //read data from ring buffer
    if (ring_buf_pop(g_rb, &sh, ring_data)) {
      
      // Check USB ready before sending
      if (!usb_cdc_ready()) {
        ESP_LOGW(TAG, "USB CDC not ready, dropping packet");
        vTaskDelay(pdMS_TO_TICKS(100));
        continue;
      }
      
      uint32_t sync_magic = SYNC_MAGIC;
      uint32_t rec_len = sh.len;
      
      size_t offset = 0;
      memcpy(tx_buf + offset, &sync_magic, sizeof(uint32_t));
      offset += sizeof(uint32_t);
      
      memcpy(tx_buf + offset, &rec_len, sizeof(uint32_t));
      offset += sizeof(uint32_t);
      
      memcpy(tx_buf + offset, ring_data, sh.len);
      offset += sh.len;
      
      size_t written = usb_cdc_write(tx_buf, offset);
      if (written > 0) {
        packets_sent++;
        if (packets_sent % 100 == 0) {
          uint16_t rb_size = ring_buf_size(g_rb);
          ESP_LOGI(TAG, "USB CDC Sent: %lu packets (Ring buffer size: %u)"
                ,packets_sent
                ,rb_size);
        }
      }
      
    } else {
      vTaskDelay(pdMS_TO_TICKS(2));
    }
  }
}


void sniffer_init(void)
{  
  // STEP.1 Create ring buffer
  uint32_t cap = have_psram() ? RING_SLOTS_PSRAM : RING_SLOTS_INTERNAL;
  ESP_ERROR_CHECK(ring_buf_create(&g_rb, cap, SLOT_SIZE, have_psram()));
  
  // STEP.2 Start streamer task
  xTaskCreate(streamer_task, "streamer", 4096, NULL, 5, NULL);

  ESP_LOGI(TAG, "Sniffer started (Ring+CDC, prefix+payload stored)");
}

void sniffer_enable_promiscuous(void)
{
    wifi_promiscuous_filter_t filt = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT |
                       WIFI_PROMIS_FILTER_MASK_DATA |
                       WIFI_PROMIS_FILTER_MASK_CTRL,
    };

    esp_err_t err = esp_wifi_set_promiscuous(false);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "set_promiscuous(false) failed: %d", err);
    }

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&filt));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous_cb));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));

    ESP_LOGI(TAG, "Promiscuous mode enabled");
}


void sniffer_ring_reset(void)
{
    if (!g_rb) return;
    ring_buf_reset(g_rb);
    captured_count = 0;
    dropped_count = 0;
}

void sniffer_print_stats(void)
{
    uint32_t sz = ring_buf_size(g_rb);
    uint32_t cap = ring_buf_cap(g_rb);

    printf("Sniffer (Ring buffer) status:\n");
    printf("  - Ring Buf: %lu / %lu used\n", sz, cap);
    printf("  - Captured: %lu\n", captured_count);
    printf("  - Dropped : %lu\n", dropped_count);
}