#include "ISRServiceable.hpp"

#include <inttypes.h>
#include "esp_log.h"

static const char *ISRServiceable_TAG = "ISRServiceable";

ISRServiceable::ISRServiceable()
    : _lastServicedTime(0)
{
}
ISRServiceable::~ISRServiceable(){};

int64_t ISRServiceable::getLastServiced() const
{
    return _lastServicedTime;
}
void ISRServiceable::setLastServiced(const int64_t lastServiced)
{
    _lastServicedTime = lastServiced;
}
