#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* 단일 슬롯 = 헤더 + payload(MAX) 크기 */
/**
 * @brief Metadata header stored immediately before each payload slot.
 * @details Upper layers reuse the magic value for integrity checks, and payload bytes follow this struct contiguously
 *          to keep DMA-friendly copies. Original inline comments remain for Korean context.
 */
typedef struct {
  uint32_t magic;     /* 패킷 식별용(상위에서 사용) */
  uint16_t len;       /* payload 길이 */
  uint16_t _rsv;      /* 정렬 */
} ring_slot_hdr_t;

typedef struct ring_buf ring_buf_t;

/* cap = 슬롯 개수, slot_size = 헤더+payload 전체 바이트 */
/**
 * @brief Create and initialise a ring buffer instance.
 * @param[out] out      Location that receives the newly-created buffer handle.
 * @param[in]  cap      Number of slots available (header + payload bundles).
 * @param[in]  slot_size Total bytes per slot including @ref ring_slot_hdr_t and payload.
 * @param[in]  try_psram Prefer allocating from PSRAM when available.
 * @return ESP_OK on success, otherwise an allocation or argument error.
 */
esp_err_t ring_buf_create(ring_buf_t **out, uint32_t cap, uint32_t slot_size, bool try_psram);
/**
 * @brief Release resources obtained from @ref ring_buf_create.
 */
void      ring_buf_destroy(ring_buf_t *rb);
/**
 * @brief Push a payload into the next free slot.
 * @details Operates non-blocking and returns false when the buffer is full so callers can track drops.
 */
bool      ring_buf_push(ring_buf_t *rb, const ring_slot_hdr_t *hdr, const uint8_t *payload);
/**
 * @brief Pop the oldest slot into the supplied buffers.
 * @details Waits up to 1 ms while acquiring the internal mutex.
 */
bool      ring_buf_pop (ring_buf_t *rb,       ring_slot_hdr_t *hdr,       uint8_t *payload);
/**
 * @brief Return the number of occupied slots.
 */
uint32_t  ring_buf_size(const ring_buf_t *rb);   /* 현재 저장된 슬롯 수 */
/**
 * @brief Return the total slot capacity.
 */
uint32_t  ring_buf_cap (const ring_buf_t *rb);   /* 총 슬롯 개수 */
/**
 * @brief Return the number of free slots remaining.
 */
uint32_t  ring_buf_free(const ring_buf_t *rb);   /* 남은 슬롯 수 */
/**
 * @brief Reset head/tail indices to empty the buffer.
 */
void      ring_buf_reset(ring_buf_t *rb);  /* 링 버퍼 초기화 */