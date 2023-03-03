#include "I2C.hpp"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c.h"
#include "soc/i2c_reg.h"

#define I2C_MASTER_TIMEOUT_MS 1000
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0

#define I2C_MASTER_SDA 6
#define I2C_MASTER_SCL 7
#define I2C_MASTER_FREQUENCY 100000
#define I2C_MASTER_PORT_NUM 0

#define I2C_LL_MAX_TIMEOUT I2C_TIME_OUT_REG_V

static bool __i2c_initialized = false;
static SemaphoreHandle_t __i2c_initialization_mutex = xSemaphoreCreateMutex();
static const char *I2C_TAG = "I2C";

#define __I2C_TIMEOUT_TICKS (I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS)

static void __init_i2c()
{
    ESP_LOGI(I2C_TAG, "ENTER: __init_i2c");
    if (!__i2c_initialized)
    {
        xSemaphoreTake(__i2c_initialization_mutex, portMAX_DELAY);
        if (!__i2c_initialized)
        {
            ESP_LOGI(I2C_TAG, "__init_i2c initializing");
            i2c_config_t conf = {
                .mode = I2C_MODE_MASTER,
                .sda_io_num = I2C_MASTER_SDA, // 8, // select GPIO specific to your project
                .scl_io_num = I2C_MASTER_SCL, // 9, // select GPIO specific to your project
                .sda_pullup_en = GPIO_PULLUP_ENABLE,
                .scl_pullup_en = GPIO_PULLUP_ENABLE,
                .master{
                    .clk_speed = I2C_MASTER_FREQUENCY,
                },                                        // select frequency specific to your project
                .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL, // you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
            };

            ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT_NUM, &conf));
            ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0));
            ESP_ERROR_CHECK(i2c_set_timeout((i2c_port_t)0, I2C_LL_MAX_TIMEOUT));
            __i2c_initialized = true;
            ESP_LOGI(I2C_TAG, "__init_i2c initialized");
        }
        xSemaphoreGive(__i2c_initialization_mutex);
    }
}

I2C::I2C(const component_id_t component, const uint8_t addr)
    : Component(component),
      _addr(addr)
{
    __init_i2c();
}

I2C::~I2C() {}

void I2C::write(const uint8_t data) const
{
    write(&data, sizeof(data));
}

void I2C::write(const uint8_t *buffer, const size_t len) const{
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_to_device(0, _addr, buffer, len, __I2C_TIMEOUT_TICKS));
}

    void I2C::writeRead(const uint8_t address, const uint8_t *writeBuffer, const size_t writeBufferLen, uint8_t *readBuffer, const size_t readBufferLen) const
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(
        I2C_MASTER_PORT_NUM,
        address,
        writeBuffer,
        writeBufferLen,
        readBuffer,
        readBufferLen,
        __I2C_TIMEOUT_TICKS));
}