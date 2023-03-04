#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class Display
{
private:
    const int16_t timeoutSeconds;
    esp_timer_handle_t dimTimer;

protected:
    const SemaphoreHandle_t displayMutex = xSemaphoreCreateRecursiveMutex();
    bool displayInitialized = false;

    void init();

protected:
    Display(int16_t sleepTimeout);

public:
    void resetSleepTimer();

    virtual void sleep() = 0;
    static void sleep(void *display);

    virtual void wakeup() = 0;
    static void wakeup(void *display);
};
