#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* 단일 슬롯 = 헤더 + payload(MAX) 크기 */
typedef struct {
  uint32_t magic;     /* 패킷 식별용(상위에서 사용) */
  uint16_t len;       /* payload 길이 */
  uint16_t _rsv;      /* 정렬 */
} ring_slot_hdr_t;

typedef struct ring_buf ring_buf_t;

/* cap = 슬롯 개수, slot_size = 헤더+payload 전체 바이트 */
esp_err_t ring_buf_create(ring_buf_t **out, uint32_t cap, uint32_t slot_size, bool try_psram);
void      ring_buf_destroy(ring_buf_t *rb);
bool      ring_buf_push(ring_buf_t *rb, const ring_slot_hdr_t *hdr, const uint8_t *payload);
bool      ring_buf_pop (ring_buf_t *rb,       ring_slot_hdr_t *hdr,       uint8_t *payload);
uint32_t  ring_buf_size(const ring_buf_t *rb);   /* 현재 저장된 슬롯 수 */
uint32_t  ring_buf_cap (const ring_buf_t *rb);   /* 총 슬롯 개수 */
uint32_t  ring_buf_free(const ring_buf_t *rb);   /* 남은 슬롯 수 */
void      ring_buf_reset(ring_buf_t *rb);  /* 링 버퍼 초기화 */