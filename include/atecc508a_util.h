#pragma once

#include <esp_err.h>

#ifndef ESP_CHECK_RET
#define ESP_CHECK_RET(f)                                                                                    \
    {                                                                                                       \
        esp_err_t _ret = (f);                                                                               \
        if (_ret != ESP_OK)                                                                                 \
        {                                                                                                   \
            ESP_LOGE(LOG_TAG, "Error %s (0x%X) at %s:%d", esp_err_to_name(_ret), _ret, __FILE__, __LINE__); \
            return _ret;                                                                                    \
        }                                                                                                   \
    }
#endif

/**
 * @brief
 *
 * @param delay
 */
void atecc508a_delay(size_t delay);

/**
 * @brief Check CRC of the responses
 *
 * @param response
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_check_crc(uint8_t *response, size_t length);