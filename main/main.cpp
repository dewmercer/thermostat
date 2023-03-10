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
#include "Led.hpp"
#include "Thermistor.hpp"
#include "ThermistorConfig.hpp"

#include "esp_log.h"
#include "driver/gpio.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "DisplayMode.hpp"
#include "MainDisplayMode.hpp"
#include "Display.hpp"
#include "Ssd1351.hpp"

#define INITIAL_SETPOINT 20

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SCLK_PIN (gpio_num_t)8
#define MOSI_PIN (gpio_num_t)10
#define DC_PIN (gpio_num_t)5
#define CS_PIN (gpio_num_t)21
#define RST_PIN (gpio_num_t)4

#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif

static const char *APP_MAIN_TAG = "APP_MAIN";
const int USED uxTopUsedPriority = configMAX_PRIORITIES - 1;

static const char *BUTTON_HANDLER_TAG = "BUTTON_HANDLER_MAIN";

static int counter8 = 0;
static void buttonHandler_6(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter8);
    DisplayMode::getActiveMode()->makeActive();
    DisplayMode::on();
}

static int counter20 = 0;
static void buttonHandler_7(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter20);
    DisplayMode::off();
}

void lcdTestPattern(void);
Adafruit_SSD1351 *tft;

Ssd1351 ssd1351(10, CS_PIN, DC_PIN, RST_PIN);

extern "C" void app_main(void)
{
    DisplayMode::init(CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN, 10);
    MainDisplayMode *disp = new MainDisplayMode();
    disp->makeActive();

    auto adc1 = new Adc(ADC_0, ADC1_CHANNEL_2);
    Thermistor t1(THERMISTOR_10K, knownThermistorConfigs["10K"], adc1);

    Led led(SANITY_LED, GPIO_NUM_9);
    led.on();

    Button b8(BUTTON_0, GPIO_NUM_6, 1, buttonHandler_6);
    ESP_LOGI(APP_MAIN_TAG, "Button B8 constructed");

    Button b20(BUTTON_1, GPIO_NUM_7, 0, buttonHandler_7);
    ESP_LOGI(APP_MAIN_TAG, "Button B20 constructed");

    disp->on();

    int setPoint = INITIAL_SETPOINT;
    int lastSetpoint = 0;
    int lastTempX10 = 0;
    while (1)
    {
        auto currentTemp = t1.getTemperature();
        if ((int)(currentTemp * 10) != lastTempX10)
        {
            lastTempX10 = (int)(currentTemp * 10);
            disp->writeTemp(currentTemp);
        }

        if (setPoint != lastSetpoint)
        {
            lastSetpoint = setPoint;
            disp->writeSetPoint(setPoint);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        led.flashNTimes(2, 100);
    }
}