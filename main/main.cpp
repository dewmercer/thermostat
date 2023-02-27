#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "Adc.hpp"
#include "Button.hpp"
#include "Component.hpp"
//#include "LiquidCrystal_I2C.hpp"
#include "Led.hpp"
//#include "Print.h"
#include "Thermistor.hpp"
#include "ThermistorConfig.hpp"

#include "esp_log.h"
#include "driver/gpio.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "Display.hpp"
#include "MainDisplay.hpp"
// #include "AM2320.hpp"

static const char *APP_MAIN_TAG = "APP_MAIN";

#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif

const int USED uxTopUsedPriority = configMAX_PRIORITIES - 1;

#define GPIO_INPUT_IO_8 (gpio_num_t)8
#define GPIO_INPUT_IO_20 (gpio_num_t)20
#define GPIO_LED (gpio_num_t)6

static const char *BUTTON_HANDLER_TAG = "BUTTON_HANDLER_MAIN";

static int counter8 = 0;
static void buttonHandler8(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter8);
    Display::on();
}

static int counter20 = 0;
static void button20Handler(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter20);
    Display::off();
}

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SCLK_PIN (gpio_num_t)8
#define MOSI_PIN (gpio_num_t)10
#define DC_PIN (gpio_num_t)5
#define CS_PIN (gpio_num_t)21
#define RST_PIN (gpio_num_t)4

/**#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
*/
#include "Fonts/FreeSerifBoldItalic9pt7b.h"

void lcdTestPattern(void);
Adafruit_SSD1351 *tft;



    extern "C" void app_main(void)
    {
    // tft = new Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

    MainDisplay *disp = new MainDisplay();
    Display::init(CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN, 10);
    disp->makeActive();

    // tft->begin();

    // tft->fillScreen(BLACK);
    //  auto lcd = LiquidCrystal_I2C(0x27, 2, 16);

    auto adc2 = new Adc(ADC_0, ADC1_CHANNEL_2);
    Thermistor t2(THERMISTOR_10K, knownThermistorConfigs["10K"], adc2);
    auto adc3 = new Adc(ADC_1, ADC1_CHANNEL_3);
    Thermistor t3(THERMISTOR_100K, knownThermistorConfigs["100K"], adc3);

    Led led(SANITY_LED, GPIO_NUM_9);
    led.on();

    Button b8(BUTTON_0, GPIO_NUM_6, 1, buttonHandler8);
    ESP_LOGI(APP_MAIN_TAG, "Button B8 constructed");

    Button b20(BUTTON_1, GPIO_NUM_7, 0, button20Handler);
    ESP_LOGI(APP_MAIN_TAG, "Button B20 constructed");

    // lcd.backlight();
    // lcd.clear();

    // auto pFont = &FreeSerifBoldItalic9pt7b;
    // tft->setFont(pFont);

    // int16_t x, y;
    // uint16_t w, h;

    // tft->getTextBounds("Temp3: ", 0, 20, &x, &y, &w, &h);
    // ESP_LOGI(APP_MAIN_TAG, "x: %" PRIu16 ", y: %" PRIu16 ", w: %" PRIu16 ", h: %" PRIu16, x, y, w, h);
    disp->on();
    while (1)
    {
        auto temp2 = t2.getTemperature();
        // tft->setCursor(0, pFont->yAdvance);
        // tft->setTextColor(RED);
        // tft->printf("Temp2: ");
        //% .1f ", temp2);
        // ESP_LOGI(APP_MAIN_TAG, "yAdvance %" PRIu8 ", x: %" PRIu16 ", y: %" PRIu16, pFont->yAdvance, tft->getCursorX(), tft->getCursorY());
        // tft->fillRect(tft->getCursorX(), tft->getCursorY() - h + 2, 127 - tft->getCursorX(), h, BLACK);
        // tft->printf("%.1f", temp2);
        disp->writeTemp(temp2);

        auto temp3 = t3.getTemperature();
        // tft->setCursor(0, 2 * pFont->yAdvance + 1);
        // tft->setTextColor(GREEN);
        // tft->printf("Temp3: ");
        //% .1f ", temp2);
        //  ESP_LOGI(APP_MAIN_TAG, "yAdvance %" PRIu8 ", x: %" PRIu16 ", y: %" PRIu16, pFont->yAdvance, tft->getCursorX(), tft->getCursorY());
        // tft->fillRect(tft->getCursorX(), tft->getCursorY() - h + 2, 127 - tft->getCursorX(), h, BLACK);
        // tft->printf("%.1f", temp3);
        // lcd.setCursor(0, 0);
        // lcd.printf("Temp2: %.1f", temp2);
        // auto temp3 = t3.getTemperature();
        // lcd.setCursor(1, 0);
        // lcd.printf("Temp3: %.1f", temp3);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // auto am2320Temp = am2320->temperature();
        // ESP_LOGI(APP_MAIN_TAG, "AM2320: %d", am2320Temp);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        led.flashNTimes(2, 100);
        }
    }

    void lcdTestPattern(void)
    {
        static const uint16_t PROGMEM colors[] =
            {RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, BLACK, WHITE};

        for (uint8_t c = 0; c < 8; c++)
        {
            tft->fillRect(0, tft->height() * c / 8, tft->width(), tft->height() / 8,
                          pgm_read_word(&colors[c]));
        }
    }
