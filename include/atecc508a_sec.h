#pragma once

#define LOCK_MODE_ZONE_CONFIG 0b10000000
#define LOCK_MODE_ZONE_DATA_AND_OTP 0b10000001
#define LOCK_MODE_SLOT0 0b10000010

esp_err_t atecc508a_lock(uint8_t zone);