#include "ISRServiceable.hpp"

ISRServiceable::ISRServiceable(const QueueHandle_t serviceQueue)
    : isrServiceQueue(serviceQueue),
      lastServicedTime(0)
{
}
ISRServiceable::~ISRServiceable(){};

int64_t ISRServiceable::lastServiced() const
{
    return lastServicedTime;
}
void ISRServiceable::lastServiced(const int64_t lastServiced)
{
    lastServicedTime = lastServiced;
}
void ISRServiceable::service() const
{
    xQueueSendFromISR(isrServiceQueue, this, NULL);
}