#include "Display.hpp"

Display::Display(int16_t timeoutSeconds)
    : timeoutSeconds(timeoutSeconds)
{
    esp_timer_create_args_t timerCreateArgs = {
        .callback = sleep,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "DisplaySleepTimer",
        .skip_unhandled_events = true};

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_create(&timerCreateArgs, &dimTimer));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, timeoutSeconds * 1000 * 1000));
};

void Display::resetSleepTimer() {
    xSemaphoreTakeRecursive(displayMutex, portMAX_DELAY);
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_stop(dimTimer));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, timeoutSeconds * 1000 * 1000));
    xSemaphoreGiveRecursive(displayMutex);
};

void Display::wakeup(void *display)
{
    xSemaphoreTakeRecursive(((Display *)display)->displayMutex, portMAX_DELAY);
    ((Display *)display)->wakeup();
    xSemaphoreGiveRecursive(((Display *)display)->displayMutex);
}

void Display::sleep(void *display)
{
    xSemaphoreTakeRecursive(((Display *)display)->displayMutex, portMAX_DELAY);
    ((Display *)display)->sleep();
    xSemaphoreGiveRecursive(((Display *)display)->displayMutex);
}
