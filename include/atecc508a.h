#pragma once

#include <esp_err.h>

#define ATECC508A_ADDR 0x60
#define ATECC508A_PORT CONFIG_ATECC508A_I2C_MASTER_PORT_NUM

typedef enum
{
    // Command executed successfully.
    ATECC508A_SUCCESS = 0x00,
    // The CheckMac or Verify command was properly sent to the device, but
    // the input client response did not match the expected value.
    ATECC508A_MISCOMPARE = 0x01,
    // Command was properly received but the length, command opcode, or
    // parameters are illegal regardless of the state (volatile and/or EEPROM
    // configuration) of the ATECC508A. Changes in the value of the command
    // bits must be made before it is re-attempted.
    ATECC508A_PARSE_ERROR = 0x03,
    // A computation error occurred during ECC processing that caused the
    // result to be invalid. Retrying the command may result in a successful
    // execution.
    ATECC508A_ECC_FAULT = 0x05,
    // Command was properly received but could not be executed by the device
    // in its current state. Changes in the device state or the value of the
    // command bits must be made before it is re-attempted.
    ATECC508A_EXEC_ERROR = 0x0F,
    // Indication that ATECC508A has received a proper Wake token.
    ATECC508A_WAKE = 0x11,
    // There is insufficient time to execute the given command before the
    // watchdog timer will expire. The system must reset the watchdog timer by
    // entering the idle or sleep modes.
    ATECC508A_WATCHDOG_EXPIRE = 0xEE,
    // Command was not properly received by ATECC508A and should be
    // re-transmitted by the I/O driver in the system. No attempt was made to
    // parse or execute the command.
    ATECC508A_ERROR = 0xFF,
} atecc508a_status_t;

typedef enum
{
    ATECC508_INFO_REVISION = 0x00,
    ATECC508_INFO_KEY_VALID = 0x01,
    ATECC508_INFO_STATE = 0x02,
    ATECC508_INFO_GPIO = 0x03
} atecc508a_info_mode_t;

typedef enum
{
    ATECC508A_ZONE_CONFIG = 0x00,
    ATECC508A_ZONE_OTP = 0x01,
    ATECC508A_ZONE_DATA = 0x02,
} atecc508a_zone_t;

typedef enum
{
    ATECC508A_ADDRESS_CONFIG_READ_BLOCK_0 = 0x0000,
    ATECC508A_ADDRESS_CONFIG_READ_BLOCK_1 = 0x0008,
    ATECC508A_ADDRESS_CONFIG_READ_BLOCK_2 = 0x0010,
    ATECC508A_ADDRESS_CONFIG_READ_BLOCK_3 = 0x0018,
} atecc508a_addr_t;

/**
 * @brief Init ATECC508A device
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_init();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_wake_up();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_sleep();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_idle();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_read_config_zone();

/**
 * @brief
 *
 * @param is_configured
 * @return esp_err_t
 */
esp_err_t atecc508a_is_configured(uint8_t *is_configured);

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_configure();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_write_config();

esp_err_t atecc508a_lock_config();

esp_err_t atecc508a_create_key_pair();

esp_err_t atecc508a_lock_data_otp();

esp_err_t atecc508a_lock_data_slot0();

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t atecc508a_random(uint8_t *random, uint8_t mode);