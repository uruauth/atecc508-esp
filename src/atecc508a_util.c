#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include "atecc508a_crc.h"

#define LOG_TAG "atecc508a"

/**
 * @brief
 *
 * @param delay
 */
void atecc508a_delay(size_t delay)
{
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

/**
 * @brief Check CRC of the responses
 *
 * @param response
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_check_crc(uint8_t *response, size_t length)
{
    uint16_t response_crc = response[length - 2] | (response[length - 1] << 8);
    uint16_t calculated_crc = atecc508a_crc(response, length - 2);

    ESP_LOGD(LOG_TAG, "Response CRC: 0x%X", response_crc);
    ESP_LOGD(LOG_TAG, "Buffer CRC: 0x%X", calculated_crc);

    return (response_crc == calculated_crc) ? ESP_OK : ESP_ERR_INVALID_CRC;
}
