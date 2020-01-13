#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "atecc508a_comm.h"
#include "atecc508a_crc.h"
#include "atecc508a_util.h"

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
    // dataOTPLockStatus = config[86], then set according to status (0x55=UNlocked, 0x00=Locked)
    // configLockStatus = config[87], then set according to status (0x55=UNlocked, 0x00=Locked)
    // slot0LockStatus = config[88], then set according to status (0x55=UNlocked, 0x00=Locked)
    if (atecc508a_config[86] == 0x00 && atecc508a_config[87] == 0x00 && atecc508a_config[88] == 0x00)
    {
        *is_configured = 1;
    }
    else
    {
        *is_configured = 0;
    }

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