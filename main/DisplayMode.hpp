#pragma once

#include "Component.hpp"
#include <map>

#include "driver/gpio.h"
#include "esp_timer.h"

#include "Adafruit_SSD1351.h"

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define ACQUIRE_ACTIVE_SCREEN \
    if (isActive)             \
    {                         \
        acquireScreen();      \
        if (isActive)         \
        {

#define RELEASE_ACTIVE_SCREEN \
    releaseScreen();          \
    }                         \
    }

typedef struct
{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} rectangle;

class DisplayMode : public Component
{
    private:
        void makeInactive();


    protected:
        bool isActive = false;


        static Adafruit_SSD1351 *getOled();
        static void acquireScreen();
        static void releaseScreen();

    public:
        DisplayMode(const component_id_t mode);
        virtual ~DisplayMode();
        virtual void makeActive();

        static void fillRect(const rectangle rect, const uint16_t color);

        static rectangle getNewBounds(const char *buffer, const int16_t x, const int16_t y);

        static void init(gpio_num_t cs_pin,
                         gpio_num_t dc_pin,
                         gpio_num_t mosi_pin,
                         gpio_num_t sclk_pin,
                         gpio_num_t rst_pin,
                         uint32_t timeoutSeconds);

        static DisplayMode *getActiveMode();

        static void resetDimTimeout();
        static void resetDimTimeout(const void *) { resetDimTimeout(); };

        static void on();
        static void on(const void *) { on(); };

        static void off();
        static void off(void *) { off(); };

        static void prepForPrint(const int16_t x, const int16_t y, const GFXfont *font, const uint16_t color);
};
