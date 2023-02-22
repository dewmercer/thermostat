#pragma once

#include "Component.hpp"

#include "driver/gpio.h"

class Led:public Component
{
private:
    gpio_num_t _pin;
    bool _onState = true;
    bool _currentState = false;

public:
    Led(const thermostat_component_id_t id, const gpio_num_t pin, const bool onState = true);
    void on();
    void off();
    void toggle();
    bool getState() const;
    void flash(const int flashDelay = 100);
    void flashNTimes(const int n, const int flashDelay = 100);
};