#include "Ssd1351.hpp"

#include "SPI.h"

#define DISPLAY_ENHANCEMENT (uint8_t)0xB2
#define ENHANCE_DIAPLAY (uint8_t)0xA4

Ssd1351::Ssd1351(uint64_t displayTimeout, gpio_num_t cs_pin, gpio_num_t dc_pin, gpio_num_t rst_pin)
    : Display(displayTimeout),
      Adafruit_SSD1351(SSD1351WIDTH, SSD1351HEIGHT, new SPIClass(), cs_pin, dc_pin, rst_pin)
{
    begin();
}

void Ssd1351::sleep()
{
    xSemaphoreTakeRecursive(displayMutex, portMAX_DELAY);
    enableDisplay(false);
    xSemaphoreGiveRecursive(displayMutex);
}

void Ssd1351::wakeup() 
{
    xSemaphoreTakeRecursive(displayMutex, portMAX_DELAY);
    enableDisplay(true);
    xSemaphoreGiveRecursive(displayMutex);
}