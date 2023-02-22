#include "Thermistor.hpp"

#include <cstring>
#include <math.h>

#include "esp_adc_cal.h"
#include "esp_log.h"

#include "driver/adc.h"
#include "driver/gpio.h"

#define KELVIN_OFFSET 273.15
#define ln(x) log(x)

// 12 bits of ADC Resolution
#define ADC_BITS 12
#define KELVIN_OFFSET 273.15
#define CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP

static const char *THERMISTOR_TAG = "THERMISTOR";

void __init_thermistor() {}

Thermistor::Thermistor(const thermostat_component_id_t id, const ThermistorConfig &config, const Adc *adc)
    : Component(id),
      _adc(adc)
{
    __init_thermistor();
    ThermistorConfig cfg{
        .name = new std::string(*config.name),
        .rDivider = config.rDivider,
        .A = config.A,
        .B = config.B,
        .C = config.C,
        .R25 = config.R25,
        .Beta = config.Beta,
        .vRef = config.vRef,
    };
    _pConfig = (pThermistorConfig)malloc(sizeof(ThermistorConfig));
    memcpy(_pConfig, &cfg, sizeof(ThermistorConfig));
}

Thermistor::~Thermistor()
{
    delete _pConfig->name;
    delete _pConfig;
}

float Thermistor::getTemperature(const bool preferCalibrated) const
{
    auto adcVolts = _adc->getMilliVolts(preferCalibrated);
    float V = adcVolts / 1000.0;
    float i = (_pConfig->vRef - V) / _pConfig->rDivider;
    float R = V / i;
    float temp = 1.0 / (_pConfig->A + _pConfig->B * ln(R) + _pConfig->C * std::pow(ln(R), 3.0)) - KELVIN_OFFSET;
    ESP_LOGI(THERMISTOR_TAG, "%s: ADC Volts: %.0f, Temp: %.1fC", _pConfig->name->c_str(), adcVolts, temp);
    return temp;
    // float BetaTemp = _pConfig->Beta / (ln(R / (_pConfig->R25 * std::exp(-1.0 * (_pConfig->Beta / (25 + KELVIN_OFFSET)))))) - KELVIN_OFFSET;
}
