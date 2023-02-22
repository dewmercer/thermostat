#pragma once

#include "Component.hpp"
#include "ISRServiceable.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

class ButtonISRHandlers;


class Button: public Component, public ISRServiceable
{
private:
  typedef void buttonHandler(Button *);
  typedef buttonHandler *pButtonHandler;

  gpio_num_t _pin;
  uint8_t _pressedState;
  pButtonHandler _handler;

public:
  friend class ButtonISRHandlers;

  Button(const thermostat_component_id_t id, const gpio_num_t pin, const uint8_t pressedState, const pButtonHandler handler);
  ~Button();

  void replaceHandler(const pButtonHandler handler);

private:
  static void __init_buttons();
};
