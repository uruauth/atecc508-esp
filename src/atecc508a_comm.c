#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "atecc508a_comm.h"
#include "atecc508a_crc.h"
#include "atecc508a_pwr.h"
#include "atecc508a_util.h"

#define LOG_TAG "atecc508a"

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
esp_err_t atecc508a_send_command(atecc508a_command_t command, uint8_t param1, uint16_t param2, uint8_t *data, size_t length)
{
    ESP_LOGD(LOG_TAG, "> Sending command 0x%02X param1=0x%02X, param2=0x%04X", command, param1, param2);

    // build packet array (total_transmission) to send a communication to IC, with opcode COMMAND
    // It expects to see: word address, count, command opcode, param1, param2, data (optional), CRC[0], CRC[1]
    uint8_t total_transmission_length = 1 + 1 + 1 + sizeof(param1) + sizeof(param2) + length + sizeof(uint16_t);

    uint8_t header[] = {
        0x03,
        total_transmission_length - 1,
        command,
        param1,
        (param2 & 0xFF),
        (param2 >> 8)};

    uint16_t crc;

    // calculate packet CRC
    atecc508a_crc_ctx_t ctx;
    atecc508a_crc_begin(&ctx);

    atecc508a_crc_update(&ctx, header + 1, sizeof(header) - 1);
    atecc508a_crc_update(&ctx, data, length);

    atecc508a_crc_end(&ctx, &crc);

    // wake up the device
    ESP_CHECK_RET(atecc508a_wake_up());

    ESP_LOGD(LOG_TAG, "> Sending command");

    // send the data
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    // address
    i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_WRITE, 1);

    // header
    i2c_master_write(cmd, header, sizeof(header), 1);

    // data
    if (data != NULL)
    {
        i2c_master_write(cmd, data, length, 1);
    }

    // crc
    i2c_master_write(cmd, (uint8_t *)&crc, sizeof(uint16_t), 1);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Error sending command to device");
        return ret;
    }

    ESP_LOGD(LOG_TAG, "> Command sent");

    return ESP_OK;
}

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_receive(uint8_t *buffer, size_t length)
{
    size_t received = 0;
    uint8_t retries = 0;

    while (length > 0)
    {
        uint8_t requestLength = length > 32 ? 32 : length;

        esp_err_t ret;

        {
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();

            i2c_master_start(cmd);

            i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_READ, 0);

            if (requestLength > 1)
            {
                i2c_master_read(cmd, buffer + received, requestLength - 1, 0);
            }
            i2c_master_read_byte(cmd, buffer + received + requestLength - 1, 1);

            i2c_master_stop(cmd);

            ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

            i2c_cmd_link_delete(cmd);
        }

        if (ret == ESP_OK)
        {
            received += requestLength;
            length -= requestLength;
        }

        if (retries++ >= 20)
        {
            return ESP_FAIL;
        }
    }

    ESP_LOGD(LOG_TAG, "< Response received (%d bytes, %d retries)", received, retries);
    ESP_LOG_BUFFER_HEX_LEVEL(LOG_TAG, buffer, received, ESP_LOG_DEBUG);

    return ESP_OK;
}

esp_err_t atecc508a_read(uint8_t zone, uint16_t address, uint8_t *buffer, uint8_t length)
{
    // adjust zone as needed for whether it's 4 or 32 bytes length read
    // bit 7 of zone needs to be set correctly
    // (0 = 4 Bytes are read)
    // (1 = 32 Bytes are read)
    if (length == 32)
    {
        zone |= 0b10000000; // set bit 7
    }
    else if (length == 4)
    {
        zone &= ~0b10000000; // clear bit 7
    }
    else
    {
        return ESP_ERR_INVALID_ARG; // invalid length, abort.
    }

    ESP_CHECK_RET(atecc508a_send_command(ATECC508A_CMD_READ, zone, address, NULL, 0));

    atecc508a_delay(1);

    uint8_t tmp_buf[35];

    ESP_CHECK_RET(atecc508a_receive(tmp_buf, sizeof(tmp_buf)));

    ESP_CHECK_RET(atecc508a_idle());

    ESP_CHECK_RET(atecc508a_check_crc(tmp_buf, sizeof(tmp_buf)));

    // copy response
    memcpy(buffer, tmp_buf + 1, length);

    return ESP_OK;
}

esp_err_t atecc508a_write()
{
    return ESP_OK;
}
