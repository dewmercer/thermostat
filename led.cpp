#include "led.h"

Led::Led(char* name, int pin, PinStatus onState)
  : _name(name), _pin(pin), _onState(onState) {
  _offState = onState == HIGH ? LOW : HIGH;
  pinMode(_pin, OUTPUT);
  off();
}

void Led::on() {
  digitalWrite(_pin, _onState);
  _currentState = _onState;
}

void Led::off() {
  digitalWrite(_pin, _offState);
  _currentState = _offState;
}

void Led::toggle() {
  _currentState == _onState ? off() : on();
}

const char* Led::name() const {
  return _name;
}
