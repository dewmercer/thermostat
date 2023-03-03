#pragma once

#include <map>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef enum
{
    ADC_0,
    ADC_1,
    ADC_2,
    ADC_3,
    BUTTON_0,
    BUTTON_1,
    SANITY_LED,
    THERMISTOR_10K,
    THERMISTOR_100K,
    SENSOR_AM2320,
    DISPLAY_LCD,
    MAIN_DISPLAY_MODE,
    NULL_DISPLAY_MODE,
} component_id_t;

class Component
{
private:
    const component_id_t _id;

public:
    Component(const component_id_t id);
    ~Component();

    component_id_t id() const;

    static bool registered(const component_id_t id);
    static Component *getComponent(const component_id_t id);

private:
    static void add(const component_id_t id, Component *component);
    static void remove(const component_id_t id);
};
