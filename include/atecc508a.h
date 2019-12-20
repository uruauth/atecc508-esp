#pragma once

#include <esp_err.h>

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
 * @brief Verify a MAC calculated on another CryptoAuthentication device.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_check_mac();

/**
 * @brief Read one of the monotonic counters
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_get_counter();

/**
 * @brief Increment one of the monotonic counters
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_inc_counter();

/**
 * @brief Derive a target key value from the target or parent key.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_derive_key();

/**
 * @brief Generate an ECDH master secret using stored private key and input public key.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_ecdh();

/**
 * @brief Generate a data digest from a random or input seed and a key.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_gen_dig();

/**
 * @brief Generate an ECC public key. Optionally generate an ECC private key.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_gen_key();

/**
 * @brief Calculate response from key and other internal data using HMAC/SHA-256.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_hmac();

/**
 * @brief Return device state information.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_get_info(atecc508a_info_mode_t mode);

/**
 * @brief Prevent further modifications to a zone of the device.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_lock();

/**
 * @brief Calculate response from key and other internal data using SHA-256.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_mac();

/**
 * @brief Generate a 32-byte random number and an internally stored Nonce.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_nonce();

/**
 * @brief Selectively put just one device on a shared bus into the idle mode.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_pause();

/**
 * @brief Write an ECC private key into a slot in the Data zone.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_priv_write();

/**
 * @brief Generate a random number.
 * 
 * @param random pointer to resulting random value
 * @param update_seed controls the mechanism of the internal RNG and seed update.
 * @return esp_err_t 
 */
esp_err_t atecc508a_random(uint32_t *random, uint8_t update_seed);

/**
 * @brief Read four bytes from the device, with or without authentication and encryption.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_read();

/**
 * @brief ECDSA signature calculation.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_sign();

/**
 * @brief Computes a SHA-256 or HMAC digest for general purpose use by the system.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_sha();

/**
 * @brief Update bytes 84 or 85 within the Configuration zone after the Configuration zone is locked.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_update_extra();

/**
 * @brief ECDSA verify calculation.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_verify();

/**
 * @brief Write 4 or 32 bytes to the device, with or without authentication and encryption.
 * 
 * @return esp_err_t 
 */
esp_err_t atecc508a_write();
