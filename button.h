#pragma once
#include <Arduino.h>

class Button {

private:
  char* _name;
  int _pin;
  PinStatus _pressedState;
  int _delay;

public:
  Button(char* name, int pin, PinStatus pressedState, int delay);
  const bool pressed() const;
  const char* name() const;
};
