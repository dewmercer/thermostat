#include "Display.hpp"

#include <map>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

static Adafruit_SSD1351 *display;
static SemaphoreHandle_t __display_mutex = xSemaphoreCreateMutex();
static bool __display_initialized = false;
static std::map<display_mode_t, Display *> modeToDisplayMap;
static uint32_t dimTimeoutSeconds;
static esp_timer_handle_t dimTimer;

#define SLEEP_IN (uint8_t)0xAE
#define SLEEP_OUT (uint8_t)0xAF

Adafruit_SSD1351 *Display::getDisplay() const{
    return display;
}

Display::Display(const display_mode_t mode)
    : Component(DISPLAY_MAIN),
      mode(mode),
      isActive(false)
{
    modeToDisplayMap[mode] = this;
}

Display::~Display(){};

void Display::makeActive()
{
    isActive = true;
    Display::on();
}

void Display::makeInactive()
{
    isActive = false;
}

void Display::init(gpio_num_t cs_pin,
                   gpio_num_t dc_pin,
                   gpio_num_t mosi_pin,
                   gpio_num_t sclk_pin,
                   gpio_num_t rst_pin,
                   uint32_t timeoutSeconds)
{
    if (!__display_initialized)
    {
        xSemaphoreTake(__display_mutex, portMAX_DELAY);
        if (!__display_initialized)
        {
            dimTimeoutSeconds = timeoutSeconds;
            display = new Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, (int8_t)cs_pin, (int8_t)dc_pin, (int8_t)mosi_pin, (int8_t)sclk_pin, (int8_t)rst_pin);
            display->begin();

            esp_timer_create_args_t timerCreateArgs = {
                .callback = off,
                .arg = NULL,
                .dispatch_method = ESP_TIMER_TASK,
                .name = "DisplayDimTimer",
                .skip_unhandled_events = true};

            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_create(&timerCreateArgs, &dimTimer));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, dimTimeoutSeconds * 1000 * 1000));
            __display_initialized = true;
        }
        xSemaphoreGive(__display_mutex);
    }
}

Display *Display::getDisplay(const display_mode_t mode)
{
    return modeToDisplayMap[mode];
}

void Display::resetDimTimeout()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_stop(dimTimer));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, dimTimeoutSeconds * 1000 * 1000));
}

void Display::on()
{
    display->sendCommand(SLEEP_OUT);
    Display::resetDimTimeout();
    display->fillScreen(WHITE);
}

void Display::off()
{
    display->sendCommand(SLEEP_IN);
}
