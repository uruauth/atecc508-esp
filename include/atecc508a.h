#pragma once

#include <esp_err.h>

typedef enum
{
    ATECC508A_CMD_RANDOM = 0x1B,
} atecc508a_command_t;

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
esp_err_t atecc508a_random(uint8_t *random, uint8_t mode);