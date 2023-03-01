#pragma once

#include "Component.hpp"
#include <map>

#include "driver/gpio.h"
#include"esp_timer.h"

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
    releaseScreen();   \
    }                  \
    }

typedef enum
{
    UNKNOWN_MODE,
    MAIN_DISPLAY
} display_mode_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} rectangle;

class Display : public Component
{
    protected:
        const display_mode_t mode;
        bool isActive = false;

        static Adafruit_SSD1351 *getDisplay();

        static void acquireScreen();
        static void releaseScreen();

    public:
        Display(const display_mode_t mode);
        virtual ~Display();

    public:
        virtual void makeActive();
        virtual void makeInactive();

        static void fillRect(const rectangle rect, const uint16_t color);

        static rectangle getNewBounds(const char *buffer, const int16_t x, const int16_t y);

        static void init(gpio_num_t cs_pin,
                         gpio_num_t dc_pin,
                         gpio_num_t mosi_pin,
                         gpio_num_t sclk_pin,
                         gpio_num_t rst_pin,
                         uint32_t timeoutSeconds);

        static Display *getDisplay(const display_mode_t mode);

        static void resetDimTimeout();
        static void resetDimTimeout(const void *) { resetDimTimeout(); };

        static void on();
        static void on(const void *) { on(); };

        static void off();
        static void off(void *) { off(); };

        static void prepForPrint(const int16_t x, const int16_t y, const GFXfont *font, const uint16_t color);
};
