#pragma once

#include <string>
#include <vector>

#include "Adc.hpp"
#include "Component.hpp"
#include "ThermistorConfig.hpp"

#include "driver/adc.h"

class Thermistor : public Component
{

private:
    typedef void changeAction(uint16_t temp);
    typedef changeAction *pChangeAction;

    pThermistorConfig _pConfig;
    const Adc *_adc;

public:
    Thermistor(const component_id_t id, const ThermistorConfig &config, const Adc *adc);
    Thermistor(const component_id_t id, const pThermistorConfig config, const Adc *adc)
        : Thermistor(id, *config, adc){};
    ~Thermistor();

    float getTemperature(const bool preferCalibrated = true) const;
};