#include "Adc.hpp"

#include <cmath>
#include <cstring>
#include <map>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_adc_cal.h"

#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define ADC_WIDTH ADC_WIDTH_BIT_12
#define ADC_ATTENUATION ADC_ATTEN_DB_11
#define VCC 3300.0

static esp_adc_cal_characteristics_t __adc1_chars;
static bool __adc_cali_enabled = false;
static bool __adc_initialized = false;
static SemaphoreHandle_t __adc_initialization_mutex = xSemaphoreCreateMutex();
static const char *ADC_TAG = "ADC";

static std::map<const adc_atten_t, const float> __attenToMaxV{
    {ADC_ATTEN_DB_0,
     950.0},
    {ADC_ATTEN_DB_2_5,
     1250.0},
    {ADC_ATTEN_DB_6,
     1750.0},
    {ADC_ATTEN_DB_11,
     2450.0}};

static void __init_adc()
{
    if (!__adc_initialized)
    {
        xSemaphoreTake(__adc_initialization_mutex, portMAX_DELAY);
        if (!__adc_initialized)
        {
            switch (esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME))
            {
            case ESP_ERR_NOT_SUPPORTED:
                ESP_LOGW(ADC_TAG, "Calibration scheme not supported, skip software calibration");
                break;
            case ESP_ERR_INVALID_VERSION:
                ESP_LOGW(ADC_TAG, "eFuse not burnt, skip software calibration");
                break;
            case ESP_OK:
                __adc_cali_enabled = true;
                esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTENUATION, ADC_WIDTH_BIT_12, 0, &__adc1_chars);
                ESP_LOGI(ADC_TAG, "ADC Calibrated");
                break;
            default:
                ESP_LOGE(ADC_TAG, "Invalid arg");
                break;
            }
            __adc_initialized = true;
        }
        xSemaphoreGive(__adc_initialization_mutex);
    }
}

Adc::Adc(component_id_t id, adc1_channel_t channel)
    : Component(id),
      _channel(channel),
      _dMax((float)pow(2, (int)ADC_WIDTH) - 1.0)
{
    __init_adc();
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH));
    ESP_ERROR_CHECK(adc1_config_channel_atten(channel, ADC_ATTENUATION));
}

Adc::~Adc()
{
}

uint32_t Adc::getRaw() const
{
    return (uint32_t)adc1_get_raw(_channel);
}

float Adc::getMilliVolts(bool preferCalibrated) const
{
    float v = 0.0;
    uint32_t raw = getRaw();
    if (preferCalibrated && __adc_cali_enabled)
    {
        return (float)esp_adc_cal_raw_to_voltage(raw, &__adc1_chars);
    }
    else
    {
        return (float)raw * __attenToMaxV[ADC_ATTENUATION] / _dMax;
    }
    return v;
}