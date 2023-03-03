#pragma once

#include "Component.hpp"

#include "driver/adc.h"

class Adc : public Component
{
private:
    adc1_channel_t _channel;
    uint16_t _dMax;

public:
    Adc(const component_id_t id, const adc1_channel_t channel);
    ~Adc();

    uint32_t getRaw() const;
    float getMilliVolts(bool preferCalibrated = true) const;
};
