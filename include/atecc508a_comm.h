#pragma once

typedef enum
{
    ATECC508A_CMD_READ = 0x02,
    ATECC508A_CMD_RANDOM = 0x1B,
} atecc508a_command_t;

/**
 * @brief
 *
 * @param command
 * @param param1
 * @param param2
 * @param data
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_send_command(atecc508a_command_t command, uint8_t param1, uint16_t param2, uint8_t *data, size_t length);

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_receive(uint8_t *buffer, size_t length);