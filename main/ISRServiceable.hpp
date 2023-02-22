#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class ISRServiceable{
    protected:
        QueueHandle_t isrServiceQueue;
        int64_t lastServicedTime;

    public:
        ISRServiceable(const QueueHandle_t serviceQueue);
        ~ISRServiceable();

        int64_t lastServiced() const;
        void lastServiced(const int64_t lastServiced);
        void service() const;
};
