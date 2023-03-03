#include "Led.hpp"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Led::Led(const component_id_t id, const gpio_num_t pin, const bool onState)
    : Component(id),
    _pin(pin), 
    _onState(onState), 
    _currentState(!_onState)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_direction(pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(pin, !onState));
}

void Led::on()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(_pin, _onState));
    _currentState = _onState;
}

void Led::off()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(_pin, !_onState));
    _currentState = !_onState;
}

void Led::toggle()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(_pin, !_currentState));
    _currentState = !_currentState;
}

bool Led::getState() const
{
    return _currentState;
}

void Led::flash(const int flashDelay)
{
    toggle();
    vTaskDelay(pdMS_TO_TICKS(flashDelay));
    toggle();
}

void Led::flashNTimes(const int n, const int flashDelay)
{
    for (int i = 0; i < n; ++i)
    {
        flash(flashDelay);
        vTaskDelay(pdMS_TO_TICKS(flashDelay));
    }
}