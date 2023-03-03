#include "DisplayMode.hpp"
#include <mutex>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "NullDisplayMode.hpp"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

static Adafruit_SSD1351 *oled;
static SemaphoreHandle_t __oled_mutex = xSemaphoreCreateMutex();
static bool __display_initialized = false;
static uint32_t dimTimeoutSeconds;
static esp_timer_handle_t dimTimer;
static DisplayMode *activeMode;

#define SLEEP_IN (uint8_t)0xAE
#define SLEEP_OUT (uint8_t)0xAF
#define DISPLAY_ENHANCEMENT (uint8_t)0xB2
#define ENHANCE_DIAPLAY (uint8_t)0xA4

static const char *TAG = "DisplayMode";
static bool isOn = false;

Adafruit_SSD1351 *DisplayMode::getOled()
{
    return oled;
}

DisplayMode::DisplayMode(const component_id_t mode)
    : Component(mode),
      isActive(false)
{
}

DisplayMode::~DisplayMode(){};

void DisplayMode::makeActive()
{
    ESP_LOGI(TAG, "makeActive");
    if (!isActive)
    {
        xSemaphoreTake(__oled_mutex, portMAX_DELAY);
        if (!isActive)
        {
            getActiveMode()->makeInactive();
            activeMode = this;
            isActive = true;
        }
        xSemaphoreGive(__oled_mutex);
    }
}

void DisplayMode::makeInactive()
{
    ESP_LOGI(TAG, "makeInactive");
    isActive = false;
}

void DisplayMode::init(gpio_num_t cs_pin,
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
            activeMode = new NullDisplayMode();
            ((NullDisplayMode*)activeMode)->isActive = true;
            dimTimeoutSeconds = timeoutSeconds;
            oled = new Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, (int8_t)cs_pin, (int8_t)dc_pin, (int8_t)mosi_pin, (int8_t)sclk_pin, (int8_t)rst_pin);
            oled->begin();
            uint8_t displayEnhancement[3] = {ENHANCE_DIAPLAY, 0x00, 0x00};
            oled->sendCommand(DISPLAY_ENHANCEMENT, displayEnhancement, sizeof(displayEnhancement));

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
        releaseScreen();
    }
}

void DisplayMode::resetDimTimeout()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_stop(dimTimer));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_timer_start_periodic(dimTimer, dimTimeoutSeconds * 1000 * 1000));
}

void DisplayMode::on()
{
    if (!isOn)
    {
        ESP_LOGI(TAG, "enter on");
        acquireScreen();
        if (!isOn)
        {
            oled->sendCommand(SLEEP_OUT);
            isOn = true;
        }
        releaseScreen();
        ESP_LOGI(TAG, "exit on");
    }
    DisplayMode::resetDimTimeout();
}

void DisplayMode::off()
{
    if (isOn)
    {
        acquireScreen();
        if (isOn)
        {
            oled->sendCommand(SLEEP_IN);
            isOn = false;
        }
        releaseScreen();
    }
}

void DisplayMode::acquireScreen()
{
    xSemaphoreTake(__oled_mutex, portMAX_DELAY);
}
void DisplayMode::releaseScreen()
{
    xSemaphoreGive(__oled_mutex);
}

void DisplayMode::fillRect(const rectangle rect, const uint16_t color)
{
    DisplayMode::getOled()->fillRect(rect.x, rect.y, rect.w, rect.h, color);
}

rectangle DisplayMode::getNewBounds(const char *buffer, const int16_t x, const int16_t y)
{
    rectangle newBounds;
    DisplayMode::getOled()->getTextBounds(buffer, x, y, &newBounds.x, &newBounds.y, &newBounds.w, &newBounds.h);
    return newBounds;
}

void DisplayMode::prepForPrint(const int16_t x, const int16_t y, const GFXfont *font, const uint16_t color)
{
    getOled()->setTextColor(color);
    getOled()->setFont(font);
    getOled()->setCursor(x, y);
}

DisplayMode * DisplayMode::getActiveMode() { return activeMode; };