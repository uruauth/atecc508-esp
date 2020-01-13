#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "atecc508a_util.h"
#include "atecc508a_comm.h"

#define LOG_TAG __FUNCTION__

esp_err_t atecc508a_wake_up()
{
    ESP_LOGD(LOG_TAG, "> Wake Up");

    // set up to write to address "0x00"
    // This creates a "wake condition" where SDA is held low for at least tWLO
    // tWLO means "wake low duration" and must be at least 60 uSeconds (which is acheived by writing 0x00 at 100KHz I2C)
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0, 0);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to execute I2C comman");
        return ret;
    }

    // some delay
    // 1500 uSeconds is minimum and known as "Wake High Delay to Data Comm." tWHI, and SDA must be high during this time.
    atecc508a_delay(2);

    // Try to read back from the ATECC508A
    // 1 byte + status + CRC
    uint8_t response[1 + 3];

    ESP_CHECK_RET(atecc508a_receive(response, sizeof(response)));

    // check crc and wake up value
    ESP_CHECK_RET(atecc508a_check_crc(response, sizeof(response)));

    if (response[1] != ATECC508A_WAKE)
    {
        ESP_LOGE(LOG_TAG, "Failed to wake up");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t atecc508a_sleep()
{
    ESP_LOGD(LOG_TAG, "> Sleep");

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, 0x01, 1);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Error entering sleep mode");
        return ret;
    }

    return ESP_OK;
}

esp_err_t atecc508a_idle()
{
    ESP_LOGD(LOG_TAG, "> Idle");

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, 0x02, 1);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Error entering idle mode");
        return ret;
    }

    return ESP_OK;
}
