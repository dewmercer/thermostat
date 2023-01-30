#include "button.h"


Button::Button(char* name, int pin, PinStatus pressedState, int delay)
  : _name(name),
    _pin(pin),
    _pressedState(pressedState),
    _delay(delay) {
  pinMode(_pin, INPUT);
}

const bool Button::pressed() const {
  PinStatus pinStatus1 = HIGH, pinStatus2 = LOW;

  while (pinStatus1 != pinStatus2) {
    pinStatus1 = digitalRead(_pin);
    delay(_delay);
    pinStatus2 = digitalRead(_pin);
  }
  return pinStatus1 == _pressedState;
}

const char* Button::name() const {
  return _name;
}
