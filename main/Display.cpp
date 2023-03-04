#include "Display.hpp"

Display::Display(int16_t timeoutSeconds)
    : timeoutSeconds(timeoutSeconds)
{
    init();
};

void Display::resetSleepTimer(){};

void Display::init(){
    if(!displayInitialized){
        xSemaphoreTakeRecursive(displayMutex, portMAX_DELAY);
            if(!displayInitialized){
                esp_timer_create_args_t timerCreateArgs = {
                    .callback = sleep,
                    .arg = this,
                    .dispatch_method = ESP_TIMER_TASK,
                    .name = "DisplaySleepTimer",
                    .skip_unhandled_events = true};

                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_create(&timerCreateArgs, &dimTimer));
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, timeoutSeconds * 1000 * 1000));
                displayInitialized = true;
            }
        xSemaphoreGiveRecursive(displayMutex);
    }
}

void Display::sleep(void *display)
{
    ((Display *)display)->sleep();
}

void Display::wakeup(void *display)
{
    ((Display *)display)->wakeup();
}