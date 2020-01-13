#include <esp_err.h>
#include <esp_system.h>

#include "atecc508a_crc.h"

#define ATECC508A_CRC_POLYNOM 0x8005

void atecc508a_crc_begin(atecc508a_crc_ctx_t *ctx)
{
    ctx->crc = 0x0000;
}

void atecc508a_crc_update(atecc508a_crc_ctx_t *ctx, uint8_t *data, size_t length)
{
    for (size_t counter = 0; counter < length; counter++)
    {
        for (uint8_t shift_register = 0x01; shift_register > 0x00; shift_register <<= 1)
        {
            uint8_t data_bit = (data[counter] & shift_register) ? 1 : 0;
            uint8_t crc_bit = ctx->crc >> 15;
            ctx->crc <<= 1;
            if (data_bit != crc_bit)
                ctx->crc ^= ATECC508A_CRC_POLYNOM;
        }
    }
}

void atecc508a_crc_end(atecc508a_crc_ctx_t *ctx, uint16_t *crc)
{
    *crc = ctx->crc;
}

uint16_t atecc508a_crc(uint8_t *data, size_t length)
{
    uint16_t crc;
    atecc508a_crc_ctx_t ctx;
    atecc508a_crc_begin(&ctx);
    atecc508a_crc_update(&ctx, data, length);
    atecc508a_crc_end(&ctx, &crc);
    return crc;
}
