#pragma once

#include <esp_err.h>

#ifndef ESP_CHECK_RET
#define ESP_CHECK_RET(f)                                                        \
    {                                                                           \
        esp_err_t _ret = (f);                                                   \
        if (_ret != ESP_OK)                                                     \
        {                                                                       \
            ESP_LOGE(LOG_TAG, "Error 0x%X at %s:%d", _ret, __FILE__, __LINE__); \
            return _ret;                                                        \
        }                                                                       \
    }
#endif