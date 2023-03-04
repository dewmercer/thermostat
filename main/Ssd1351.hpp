#pragma once

#include "hal/gpio_types.h"
#include "SPI.h"

#include "Display.hpp"
#include "Adafruit_SSD1351.h"

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

class Ssd1351 : public Display, public Adafruit_SSD1351
{


public:
    Ssd1351(uint64_t displayTimeout, gpio_num_t cs_pin, gpio_num_t dc_pin, gpio_num_t rst_pin = GPIO_NUM_NC)
        : Display(displayTimeout),
        Adafruit_SSD1351(SSD1351WIDTH, SSD1351HEIGHT, new SPIClass(), cs_pin, dc_pin, rst_pin)
    {
        begin();
    };

    void sleep();
    void wakeup();
};
