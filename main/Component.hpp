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
} thermostat_component_id_t;

class Component{
    private:
        const thermostat_component_id_t _id;

    public:
        Component(const thermostat_component_id_t id);
        ~Component();

        thermostat_component_id_t id() const;

        static bool
        registered(const thermostat_component_id_t id);
        static Component *get(const thermostat_component_id_t id);

    private:
        static bool add(const thermostat_component_id_t id, Component *component);
        static void remove(const thermostat_component_id_t id);
};
