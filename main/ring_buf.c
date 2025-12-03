/**
 * @file ring_buf.c
 * @brief Mutex-protected ring buffer implementation used by the Wi-Fi sniffer pipeline.
 */

#include "ring_buf.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "esp_log.h"


static const char *TAG = "RING_BUF";

struct ring_buf {
  uint32_t head, tail, cap, slot_size;
  uint8_t *slots;
  SemaphoreHandle_t lock;
};



/**
 * @brief Advance an index while wrapping at @p cap.
 */
static inline uint32_t nxt(uint32_t i, uint32_t cap){ 
  return (i+1U) % cap; 
}

esp_err_t ring_buf_create(ring_buf_t **out, uint32_t cap, uint32_t slot_size, bool try_psram)
{
  if (!out || !cap || !slot_size) 
    return ESP_ERR_INVALID_ARG;
  
  ring_buf_t *rb = heap_caps_malloc(sizeof(*rb), MALLOC_CAP_8BIT);
  
  if (!rb) 
    return ESP_ERR_NO_MEM;
  
  memset(rb, 0, sizeof(*rb));
  
  rb->cap = cap; 
  rb->slot_size = slot_size;
  rb->lock = xSemaphoreCreateMutex();

  if (!rb->lock) { 
    free(rb); 
    return ESP_ERR_NO_MEM; 
  }

  int caps = MALLOC_CAP_8BIT | (try_psram ? MALLOC_CAP_SPIRAM : 0);
  size_t bytes = (size_t)cap * slot_size;
  rb->slots = heap_caps_malloc(bytes, caps);
  
  if (!rb->slots) { 
    vSemaphoreDelete(rb->lock); 
    free(rb); 
    return ESP_ERR_NO_MEM; 
  }
  *out = rb; 
  return ESP_OK;
}

void ring_buf_destroy(ring_buf_t *rb)
{
  if (!rb) return;
  if (rb->slots) free(rb->slots);
  if (rb->lock) vSemaphoreDelete(rb->lock);
  free(rb);  
}

bool ring_buf_push(ring_buf_t *rb, const ring_slot_hdr_t *hdr, const uint8_t *payload)
{
  if (!rb || !hdr || !payload) return false;
  bool ok=false;

  if (xSemaphoreTake(rb->lock, 0) == pdTRUE) {    
    uint32_t next = nxt(rb->head, rb->cap);
    if (next != rb->tail) {
      uint8_t *slot = rb->slots + ((size_t)rb->head * rb->slot_size);
      memcpy(slot, hdr, sizeof(*hdr));
      memcpy(slot + sizeof(*hdr), payload, hdr->len);
      rb->head = next; 
      ok=true;
    }
    xSemaphoreGive(rb->lock);
  }
  return ok;
}

bool ring_buf_pop(ring_buf_t *rb, ring_slot_hdr_t *hdr, uint8_t *payload)
{
  if (!rb || !hdr || !payload) return false;
  bool ok=false;

  if (xSemaphoreTake(rb->lock, pdMS_TO_TICKS(1)) == pdTRUE) {
    if (rb->tail != rb->head) {
      uint8_t *slot = rb->slots + ((size_t)rb->tail * rb->slot_size);
      memcpy(hdr, slot, sizeof(*hdr));
      memcpy(payload, slot + sizeof(*hdr), hdr->len);
      rb->tail = nxt(rb->tail, rb->cap);
      ok=true;
    }
    xSemaphoreGive(rb->lock);
  }
  return ok;
}

uint32_t ring_buf_size(const ring_buf_t *rb)
{
  if (!rb) return 0;
  int32_t diff = (int32_t)rb->head - (int32_t)rb->tail;
  if (diff < 0) diff += (int32_t)rb->cap;
  return (uint32_t)diff;
}

uint32_t ring_buf_cap(const ring_buf_t *rb)
{
  return rb ? rb->cap : 0;
}

uint32_t ring_buf_free(const ring_buf_t *rb)
{
    if (!rb) return 0;
    return rb->cap - ring_buf_size(rb) - 1;
}

void ring_buf_reset(ring_buf_t *rb)
{
    if (!rb) return;

    if (xSemaphoreTake(rb->lock, portMAX_DELAY) == pdTRUE) {
        rb->head = 0;
        rb->tail = 0;
        // 필요하면 버퍼 내용도 지우고 싶을 때:
        // memset(rb->slots, 0, (size_t)rb->cap * rb->slot_size);
        xSemaphoreGive(rb->lock);
    }
}
