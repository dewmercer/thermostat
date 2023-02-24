#pragma once

#include "Component.hpp"

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

typedef enum
{
    MAIN_DISPLAY
} display_mode_t;

class Display: public Component{
  private:


  protected:
      const display_mode_t mode;
      bool isActive;
      

  public:
      Display(const display_mode_t mode);
      virtual ~Display();

  public:
      void makeActive();
      void makeInactive();

      static void init(gpio_num_t cs_pin,
                       gpio_num_t dc_pin,
                       gpio_num_t mosi_pin,
                       gpio_num_t sclk_pin,
                       uint32_t timeoutSeconds);

      static Display *getDisplay(const display_mode_t mode);

      static void resetDimTimeout();
      static void resetDimTimeout(const void *) { resetDimTimeout(); };

      static void on();
      static void on(const void *) { on(); };

      static void off();
      static void off(void *) { off(); };
};

static Adafruit_SSD1351 *display;
