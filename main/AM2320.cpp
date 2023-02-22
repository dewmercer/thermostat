#include "AM2320.hpp"

#include "Component.hpp"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"

#define __AM2320_SENSOR_VERSION 1 ///< the sensor version

// Registers
#define __AM2320_REG_HUM_H 0x00  ///< humidity register address
#define __AM2320_REG_HUM_L 0x01  ///< humidity register address
#define __AM2320_REG_TEMP_H 0x02 ///< temp register address
#define __AM2320_REG_TEMP_L 0x03 ///< temp register address

#define __AM2320_CMD_READREG 0x03 ///< read register command

#define __AM2320_ADDRESS 0x5C // (0xB8 >> 1)

static bool __am2320_initialized = false;
static SemaphoreHandle_t __am2320_initialization_mutex = xSemaphoreCreateMutex();
static const char *AM2320_TAG = "AM2320";

static void __init_am2320()
{
    if (!__am2320_initialized)
    {
        xSemaphoreTake(__am2320_initialization_mutex, portMAX_DELAY);
        if (!__am2320_initialized)
        {
            ESP_LOGI(AM2320_TAG, "__init_am2320 initializing");

            __am2320_initialized = true;
            ESP_LOGI(AM2320_TAG, "__init_am2320 initialized");
        }
        xSemaphoreGive(__am2320_initialization_mutex);
    }
}

AM2320::AM2320() : I2C(SENSOR_AM2320, __AM2320_ADDRESS)
{
    __init_am2320();
}

AM2320::~AM2320()
{
}

uint16_t AM2320::temperature() const
{
    uint8_t data[2] = {0};
    uint8_t registerAddress = __AM2320_REG_TEMP_H;

    writeRead(
        __AM2320_ADDRESS,
        &registerAddress,
        1,
        data,
        1);

    ESP_LOGI(AM2320_TAG, "TEMP_H = 0x%02X 0x%02X", data[0], data[1]);
    return static_cast<uint16_t>(*data);
}
