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
#define DISPLAY_ENHANCEMENT (uint8_t)0xB2
#define ENHANCE_DIAPLAY (uint8_t)0xA4

static const char *TAG = "Display";

Adafruit_SSD1351 *Display::getDisplay()
{
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
        acquireScreen();
        if (!__display_initialized)
        {
            dimTimeoutSeconds = timeoutSeconds;
            display = new Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, (int8_t)cs_pin, (int8_t)dc_pin, (int8_t)mosi_pin, (int8_t)sclk_pin, (int8_t)rst_pin);
            display->begin();
            uint8_t displayEnhancement[3] = {ENHANCE_DIAPLAY, 0x00, 0x00};
            display->sendCommand(DISPLAY_ENHANCEMENT, displayEnhancement, sizeof(displayEnhancement));
/**
            esp_timer_create_args_t timerCreateArgs = {
                .callback = off,
                .arg = NULL,
                .dispatch_method = ESP_TIMER_TASK,
                .name = "DisplayDimTimer",
                .skip_unhandled_events = true};

            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_create(&timerCreateArgs, &dimTimer));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, dimTimeoutSeconds * 1000 * 1000));
   */         __display_initialized = true;
        }
        releaseScreen();
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
    acquireScreen();
    display->sendCommand(SLEEP_OUT);
    Display::resetDimTimeout();
    display->fillScreen(WHITE);
    releaseScreen();
}

void Display::off()
{
    acquireScreen();
    display->sendCommand(SLEEP_IN);
    releaseScreen();
}

void Display::acquireScreen()
{
    xSemaphoreTake(__display_mutex, portMAX_DELAY);
}
void Display::releaseScreen()
{
    xSemaphoreGive(__display_mutex);
}

void Display::fillRect(const rectangle rect, const uint16_t color)
{
    Display::getDisplay()->fillRect(rect.x, rect.y, rect.w, rect.h, color);
}

rectangle Display::getNewBounds(const char *buffer, const int16_t x, const int16_t y)
{
    rectangle newBounds;
    Display::getDisplay()->getTextBounds(buffer, x, y, &newBounds.x, &newBounds.y, &newBounds.w, &newBounds.h);
    return newBounds;
}

void Display::prepForPrint(const int16_t x, const int16_t y, const GFXfont *font, const uint16_t color)
{
    getDisplay()->setTextColor(color);
    getDisplay()->setFont(font);
    getDisplay()->setCursor(x, y);
}
