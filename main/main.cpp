/* GPIO Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

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
#include "LiquidCrystal_I2C.hpp"
#include "Led.hpp"
#include "Print.hpp"
#include "Thermistor.hpp"
#include "ThermistorConfig.hpp"

#include "esp_log.h"
#include "driver/gpio.h"

// #include "AM2320.hpp"

// #include "Adafruit_GFX.h"

#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif

const int USED uxTopUsedPriority = configMAX_PRIORITIES - 1;

#define GPIO_INPUT_IO_8 (gpio_num_t)8
#define GPIO_INPUT_IO_20 (gpio_num_t)20
#define GPIO_LED (gpio_num_t)9

static const char *APP_MAIN_TAG = "APP_MAIN";
static const char *BUTTON_HANDLER_TAG = "BUTTON_HANDLER_MAIN";

static int counter8 = 0;
static void buttonHandler8(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter8);
}

static int counter20 = 0;
static void buttonHandler20(Button *button)
{
    ESP_LOGI(BUTTON_HANDLER_TAG, "Handeling button id: %u, %d", button->id(), ++counter20);
}

extern "C" void
app_main(void)
{

    // auto am2320 = new AM2320();

    auto lcd = LiquidCrystal_I2C(0x27, 2, 16);

    auto adc2 = new Adc(ADC_0, ADC1_CHANNEL_2);
    Thermistor t2(THERMISTOR_10K, knownThermistorConfigs["10K"], adc2);
    auto adc3 = new Adc(ADC_1, ADC1_CHANNEL_3);
    Thermistor t3(THERMISTOR_100K, knownThermistorConfigs["100K"], adc3);

    Led led(SANITY_LED, GPIO_LED);

    Button b8(BUTTON_0, GPIO_NUM_8, 0, buttonHandler8);
    ESP_LOGI(APP_MAIN_TAG, "Button B8 constructed");

    Button b20(BUTTON_1, GPIO_NUM_20, 1, buttonHandler20);
    ESP_LOGI(APP_MAIN_TAG, "Button B20 constructed");

    lcd.backlight();
    lcd.clear();

    while (1)
    {
        auto temp2 = t2.getTemperature();
        lcd.setCursor(0, 0);
        lcd.printf("Temp2: %.1f", temp2);
        auto temp3 = t3.getTemperature();
        lcd.setCursor(1, 0);
        lcd.printf("Temp3: %.1f", temp3);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // auto am2320Temp = am2320->temperature();
        // ESP_LOGI(APP_MAIN_TAG, "AM2320: %d", am2320Temp);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        led.flashNTimes(2, 100);
    }
}
