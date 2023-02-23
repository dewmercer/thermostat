#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class ISRServiceable
{
protected:
    int64_t _lastServicedTime;

public:
    ISRServiceable();
    virtual ~ISRServiceable();

    int64_t getLastServiced() const;
    void setLastServiced(const int64_t lastServiced);
};
