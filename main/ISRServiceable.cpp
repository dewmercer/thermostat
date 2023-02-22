#include "ISRServiceable.hpp"

#include <inttypes.h>
#include "esp_log.h"

static const char *ISRServiceable_TAG = "ISRServiceable";

ISRServiceable::ISRServiceable(const QueueHandle_t serviceQueue)
    : _isrServiceQueue(serviceQueue),
      _lastServicedTime(0)
{
}
ISRServiceable::~ISRServiceable(){};

int64_t ISRServiceable::lastServiced() const
{
    return _lastServicedTime;
}
void ISRServiceable::lastServiced(const int64_t lastServiced)
{
    _lastServicedTime = lastServiced;
}
void IRAM_ATTR ISRServiceable::serviceFromISR(ISRServiceable *obj)
{
    xQueueSendFromISR(obj->_isrServiceQueue, &obj, NULL);
}