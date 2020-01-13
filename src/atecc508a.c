#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "atecc508a_crc.h"
#include "atecc508a_util.h"
#include "atecc508a_comm.h"

#include "sdkconfig.h"

#define LOG_TAG "atecc508a"

//
uint8_t atecc508a_config[128];

esp_err_t atecc508a_init()
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_ATECC508A_I2C_MASTER_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = CONFIG_ATECC508A_I2C_MASTER_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = CONFIG_ATECC508A_I2C_MASTER_FREQUENCY};

    ESP_CHECK_RET(i2c_param_config(ATECC508A_PORT, &config));

    ESP_CHECK_RET(i2c_driver_install(ATECC508A_PORT, config.mode, 0, 0, 0));

    ESP_LOGI(LOG_TAG, "ATECC508A I2C Init done");

    return ESP_OK;
}


/**
 * @brief Reads data from the IC at a specific zone and address.
 *
 * @param zone
 * @param address
 * @param buffer
 * @param length
 * @return esp_err_t
 */
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

esp_err_t atecc508a_read_config_zone()
{
    ESP_ERROR_CHECK(atecc508a_read(ATECC508A_ZONE_CONFIG, ATECC508A_ADDRESS_CONFIG_READ_BLOCK_0, atecc508a_config, 32));

    ESP_ERROR_CHECK(atecc508a_read(ATECC508A_ZONE_CONFIG, ATECC508A_ADDRESS_CONFIG_READ_BLOCK_1, atecc508a_config + 32, 32));

    ESP_ERROR_CHECK(atecc508a_read(ATECC508A_ZONE_CONFIG, ATECC508A_ADDRESS_CONFIG_READ_BLOCK_2, atecc508a_config + 64, 32));

    ESP_ERROR_CHECK(atecc508a_read(ATECC508A_ZONE_CONFIG, ATECC508A_ADDRESS_CONFIG_READ_BLOCK_3, atecc508a_config + 96, 32));

    ESP_LOGD(LOG_TAG, "Config Zone:");
    ESP_LOG_BUFFER_HEX_LEVEL(LOG_TAG, atecc508a_config, 128, ESP_LOG_DEBUG);

    return ESP_OK;
}

esp_err_t atecc508a_is_configured(uint8_t *is_configured)
{
    return ESP_OK;
}

esp_err_t atecc508a_write_config()
{
    return ESP_OK;
}

esp_err_t atecc508a_lock_config()
{
    return ESP_OK;
}

esp_err_t atecc508a_create_key_pair()
{
    return ESP_OK;
}

esp_err_t atecc508a_lock_data_otp()
{
    return ESP_OK;
}

esp_err_t atecc508a_lock_data_slot0()
{
    return ESP_OK;
}

esp_err_t atecc508a_random(uint8_t *random, uint8_t mode)
{
    uint8_t response[35] = {};

    // send command
    ESP_CHECK_RET(atecc508a_send_command(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0));

    atecc508a_delay(23);

    ESP_CHECK_RET(atecc508a_receive(response, sizeof(response)));

    ESP_CHECK_RET(atecc508a_idle());

    ESP_CHECK_RET(atecc508a_check_crc(response, sizeof(response)));

    memcpy(random, response + 1, 32);

    return ESP_OK;
}