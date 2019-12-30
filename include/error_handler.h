#pragma once

#include <esp_err.h>

#ifndef ESP_CHECK_RET
#define ESP_CHECK_RET(f)                                                       \
    {                                                                          \
        esp_err_t ret = (f);                                                   \
        if (ret != ESP_OK)                                                     \
        {                                                                      \
            ESP_LOGE(LOG_TAG, "Error 0x%X at %s:%d", ret, __FILE__, __LINE__); \
            return ret;                                                        \
        }                                                                      \
    }
#endif