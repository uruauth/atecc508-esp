#pragma once

#include <esp_err.h>

#include "uru-lib.h"

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