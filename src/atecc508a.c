#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include <driver/i2c.h>

#include "atecc508a.h"
#include "error_handler.h"
#include "sdkconfig.h"

#define LOG_TAG "atecc508a"

#define ATECC508A_ADDR 0x60
#define ATECC508A_PORT CONFIG_ATECC508A_I2C_MASTER_PORT_NUM

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
 * @brief
 *
 * @param delay
 */
static void atecc508a_delay(size_t delay)
{
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

#define ATECC508A_CRC_POLYNOM 0x8005

typedef struct
{
    uint16_t crc;
} atecc508a_crc_ctx_t;

/**
 * @brief
 *
 * @param crc
 */
static void atecc508a_crc_begin(atecc508a_crc_ctx_t *ctx)
{
    ctx->crc = 0x0000;
}

/**
 * @brief
 *
 * @param crc
 * @param data
 * @param length
 */
static void atecc508a_crc_update(atecc508a_crc_ctx_t *ctx, uint8_t *data, size_t length)
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

/**
 * @brief
 *
 * @param ctx
 * @param crc
 */
static void atecc508a_crc_end(atecc508a_crc_ctx_t *ctx, uint16_t *crc)
{
    *crc = ctx->crc;
}

/**
 * @brief
 *
 * @param data
 * @param length
 * @param crc
 */
static void atecc508a_crc(uint8_t *data, size_t length, uint16_t *crc)
{
    atecc508a_crc_ctx_t ctx;
    atecc508a_crc_begin(&ctx);
    atecc508a_crc_update(&ctx, data, length);
    atecc508a_crc_end(&ctx, crc);
}

esp_err_t atecc508a_wake_up()
{
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
    uint8_t response[4];

    ESP_CHECK_RET(atecc508a_receive(response, 4));

    ESP_LOGI(LOG_TAG, "Wake up response recieved");
    ESP_LOG_BUFFER_HEX(LOG_TAG, response, 4);

    // check crc and wake up value
    uint16_t crc;
    atecc508a_crc(response, 2, &crc);
    if (crc != 0x4333 || response[1] != ATECC508A_WAKE)
    {
        ESP_LOGE(LOG_TAG, "Failed to wake up");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t atecc508a_sleep()
{
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

/**
 * @brief
 *
 * @param command
 * @return esp_err_t
 */
static esp_err_t atecc508a_send_command(atecc508a_command_t command, uint8_t param1, uint16_t param2, uint8_t *data, size_t length)
{
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

    // calculate packet CRC
    atecc508a_crc_ctx_t ctx;
    atecc508a_crc_begin(&ctx);

    atecc508a_crc_update(&ctx, header + 1, sizeof(header) - 1);
    atecc508a_crc_update(&ctx, data, length);

    // wake up the device
    ESP_CHECK_RET(atecc508a_wake_up());

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
    i2c_master_write(cmd, (uint8_t *)&ctx.crc, sizeof(uint16_t), 1);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(ATECC508A_PORT, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Error sending command to device");
        return ret;
    }

    return ESP_OK;
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

    ESP_LOGD(LOG_TAG, "%s retry count: %d", __FUNCTION__, retries);

    return ESP_OK;
}

esp_err_t atecc508a_random(uint8_t *random, uint8_t mode)
{
    // send command
    ESP_CHECK_RET(atecc508a_send_command(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0));

    atecc508a_delay(23);

    // receive the response
    uint8_t response[35] = {};

    ESP_CHECK_RET(atecc508a_receive(response, sizeof(response)));

    ESP_LOGI(LOG_TAG, "Random response received");
    ESP_LOG_BUFFER_HEX(LOG_TAG, response, sizeof(response));

    uint16_t crc;
    atecc508a_crc(response, 33, &crc);

    uint16_t ccrc = response[33] | (response[34] << 8);

    if (ccrc != crc)
    {
        ESP_LOGE(LOG_TAG, "Error receiving random value");
        return ESP_FAIL;
    }

    memcpy(random, response + 1, 32);

    return ESP_OK;
}