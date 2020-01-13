#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include "atecc508a.h"
#include "atecc508a_comm.h"
#include "atecc508a_sec.h"
#include "atecc508a_util.h"

#define LOG_TAG "atecc508a"

esp_err_t atecc508a_lock(uint8_t zone)
{
    // ESP_ERROR_CHECK(atecc508a_send_command(ATECC508A_CMD_LOCK, zone, 0x0000, NULL, 0));

    atecc508a_delay(32);

    uint8_t resp[4];

    ESP_ERROR_CHECK(atecc508a_receive(resp, sizeof(resp)));

    ESP_CHECK_RET(atecc508a_idle());

    ESP_CHECK_RET(atecc508a_check_crc(resp, sizeof(resp)));

    if (resp[1] != 0x00)
    {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

esp_err_t atecc508a_create_key_pair()
{

    return ESP_OK;
}
