#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class ISRServiceable
{
protected:
    QueueHandle_t _isrServiceQueue;
    int64_t _lastServicedTime;

public:
    ISRServiceable(const QueueHandle_t serviceQueue);
    virtual ~ISRServiceable();

    int64_t lastServiced() const;
    void lastServiced(const int64_t lastServiced);
    static void serviceFromISR(ISRServiceable *obj);
};
