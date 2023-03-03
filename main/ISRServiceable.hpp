#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class ISRServiceable
{
protected:
    int64_t _lastServicedTime;
    int64_t _isrReceivedAt;

public:
    ISRServiceable()
    : _lastServicedTime(0),
    _isrReceivedAt(0)
    {}
    
    virtual ~ISRServiceable(){};

    int64_t getLastServiced() const{
        return _lastServicedTime;
    }
    void setLastServiced(const int64_t lastServiced){
        _lastServicedTime = lastServiced;
    }
    void setLastServicedNow(){
        setLastServiced(esp_timer_get_time());
    }

    int64_t getIsrReceivedAt() const{
        return _isrReceivedAt;
    }
    void setIsrReceivedAt(const int64_t receivedAt){
        _isrReceivedAt = receivedAt;
    }
    void setIsrReceivedAtNow(){
        setIsrReceivedAt(esp_timer_get_time());
    }
};
