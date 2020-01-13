#pragma once

typedef struct
{
    uint16_t crc;
} atecc508a_crc_ctx_t;

/**
 * @brief
 *
 * @param crc
 */
void atecc508a_crc_begin(atecc508a_crc_ctx_t *ctx);

/**
 * @brief
 *
 * @param crc
 * @param data
 * @param length
 */
void atecc508a_crc_update(atecc508a_crc_ctx_t *ctx, uint8_t *data, size_t length);

/**
 * @brief
 *
 * @param ctx
 * @param crc
 */
void atecc508a_crc_end(atecc508a_crc_ctx_t *ctx, uint16_t *crc);

/**
 * @brief
 *
 * @param data
 * @param length
 * @param crc
 */
uint16_t atecc508a_crc(uint8_t *data, size_t length);