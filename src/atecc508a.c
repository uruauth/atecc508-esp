#include <esp_err.h>
#include <esp_log.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "error_handler.h"
#include "sdkconfig.h"

#define LOG_TAG "atecc508a"

#define ATECC508A_ADDR 0x60
#define ATECC508A_PORT CONFIG_ATECC508A_I2C_MASTER_PORT_NUM

#define ACK_VAL 0x0  /*!< I2C ack value */
#define NACK_VAL 0x1 /*!< I2C nack value */

esp_err_t atecc508a_init()
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_ATECC508A_I2C_MASTER_SDA,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = CONFIG_ATECC508A_I2C_MASTER_SCL,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = CONFIG_ATECC508A_I2C_MASTER_FREQUENCY};

    ESP_CHECK_RET(i2c_param_config(ATECC508A_PORT, &config));

    ESP_CHECK_RET(i2c_driver_install(ATECC508A_PORT, config.mode, 0, 0, 0));

    ESP_LOGI(LOG_TAG, "ATECC508A I2C Init done");

    return ESP_OK;
}

/**
 * @brief
 *
 * @param command
 * @return esp_err_t
 */
static esp_err_t atecc508a_send(uint8_t *buffer, size_t length)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_WRITE, 1);

    i2c_master_write(cmd, buffer, length, 1);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @return esp_err_t
 */
static esp_err_t atecc508a_receive(uint8_t *buffer, size_t length)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (ATECC508A_ADDR << 1) | I2C_MASTER_READ, ACK_VAL);

    if (length > 1)
    {
        i2c_master_read(cmd, buffer, length - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, buffer + length - 1, NACK_VAL);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t atecc508a_wake_up()
{
    // set up to write to address "0x00"
    // This creates a "wake condition" where SDA is held low for at least tWLO
    // tWLO means "wake low duration" and must be at least 60 uSeconds (which is acheived by writing 0x00 at 100KHz I2C)
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0, ACK_VAL);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    ESP_CHECK_RET(ret);

    // some delay
    // 1500 uSeconds is minimum and known as "Wake High Delay to Data Comm." tWHI, and SDA must be high during this time.
    vTaskDelay(2 / portTICK_PERIOD_MS);

    // Read back from the ATECC508A
    uint8_t buffer[4];

    ESP_CHECK_RET(atecc508a_receive(buffer, 4));

    ESP_LOG_BUFFER_HEX(LOG_TAG, buffer, 4);

    // check count

    // check crc

    // buffer[1] = 0x11;

    return ESP_OK;
}

esp_err_t atecc508a_get_info(atecc508a_info_mode_t mode)
{
    uint8_t cmd[4] = {0x30, 0x00, 0x00, 0x00};

    ESP_CHECK_RET(atecc508a_send(cmd, 4));

    uint8_t response[4];

    ESP_CHECK_RET(atecc508a_receive(response, 4));

    ESP_LOG_BUFFER_HEX(LOG_TAG, response, 4);

    return ESP_OK;
}

esp_err_t atecc508a_random(uint32_t *random, uint8_t update_seed)
{

    return ESP_OK;
}