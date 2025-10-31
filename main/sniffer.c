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


#include "sniffer.h"
#include "ring_buf.h"
#include "usb_cdc.h"


static const char *TAG = "SNIFFER";

static ring_buf_t *g_rb = NULL;
#define SLOT_SIZE (sizeof(packet_prefix_t) + MAX_PACKET_SIZE)

static bool have_psram(void){ return heap_caps_get_free_size(MALLOC_CAP_SPIRAM) > 0; }

static uint32_t captured_count = 0;
static uint32_t dropped_count = 0;

static void wifi_promiscuous_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type!=WIFI_PKT_MGMT && type!=WIFI_PKT_DATA && type!=WIFI_PKT_CTRL && type!=WIFI_PKT_MISC) return;

  captured_count++;
  
  // Log every 100 packets
  if (captured_count % 100 == 0) {
    ESP_LOGI(TAG, "Captured: %lu, Dropped: %lu", captured_count, dropped_count);
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

  ring_slot_hdr_t sh = { .magic = PACKET_HEADER_MAGIC, .len = (uint16_t)(sizeof(ph) + len) };
  
  if (!ring_buf_push(g_rb, &sh, frame)) {
    dropped_count++;
  }
}

/* ---------- streamer task: ring → CDC ---------- */
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
      ESP_LOGI(TAG, "USB CDC ready: %s, Packets sent: %lu", 
               cdc_ready ? "YES" : "NO", packets_sent);
    }
    loop_count++;
    
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
          ESP_LOGI(TAG, "Sent %lu packets", packets_sent);
        }
      }
      
    } else {
      vTaskDelay(pdMS_TO_TICKS(2));
    }
  }
}


void sniffer_init(void)
{

  // 1. Initialize NVS FIRST - THIS WAS MISSING
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  ESP_LOGI(TAG, "NVS initialized");
  
  // 2. Initialize USB CDC before WiFi
  usb_cdc_init();
  
  // 3. Create ring buffer
  uint32_t cap = have_psram() ? RING_SLOTS_PSRAM : RING_SLOTS_INTERNAL;
  ESP_ERROR_CHECK(ring_buf_create(&g_rb, cap, SLOT_SIZE, have_psram()));

  // 4. Initialize Wi-Fi in promiscuous mode
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t wcfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wcfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb));

  // 5. Start streamer task
  xTaskCreate(streamer_task, "streamer", 4096, NULL, 5, NULL);

  ESP_LOGI(TAG, "Sniffer started (ring+CDC, prefix+payload stored)");
}

/* Keep app_main here for single-file main; alternatively, create a separate main.c */
void app_main(void) {
  sniffer_init();
}
