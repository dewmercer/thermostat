#pragma once
#include <Arduino.h>

class Led {

private:
  char* _name;
  int _pin;
  PinStatus _onState;
  PinStatus _offState;
  PinStatus _currentState;

public:
  Led(char* name, int pin, PinStatus onState);
  Led(int pin, PinStatus onState)
    : Led("", pin, onState) {}
  Led(char* name, int pin)
    : Led(name, pin, HIGH) {}
  Led(int pin)
    : Led("", pin) {}
  void on();
  void off();
  void toggle();
  const char* name() const;
};
