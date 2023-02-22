#include "Button.hpp"

#include <map>
#include <mutex>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "driver/gpio.h"

// 150ms
#define DEBOUNCE_TIME (uint64_t)150000

static QueueHandle_t __button_isr_event_queue = NULL;
static SemaphoreHandle_t __button_container_semaphore = NULL;

static const char *ISR_TAG = "Button ButtonIsr";
static const char *BUTTON_ACTION_HANDLER_TAG_TAG = "ButtonActionHandler";

class ButtonISRHandlers
{
public:
  friend class Button;

private:
  static void IRAM_ATTR ButtonIsr(ISRServiceable *button)
  {
    ESP_DRAM_LOGD(ISR_TAG, "Enter");
    if (button->lastServiced() + DEBOUNCE_TIME <= esp_timer_get_time())
    {
      button->lastServiced(esp_timer_get_time());
      button->service();
      ESP_DRAM_LOGI(ISR_TAG, "No Bounce pin: %d", static_cast<Button*>(button)->_pin);
    }
    ESP_DRAM_LOGD(ISR_TAG, "Exit");
  }

  static void ButtonActionHandler(void *arg)
  {
    ESP_LOGI(BUTTON_ACTION_HANDLER_TAG_TAG, "dispatcher running");
    for (;;)
    {
      ESP_LOGD(BUTTON_ACTION_HANDLER_TAG_TAG, "WAITING");
      Button *button;
      if (xQueueReceive(__button_isr_event_queue, &button, portMAX_DELAY))
      {
        button->_handler(button);
      }
    }
  }
};

static const char *BUTTON_TAG = "Button";
static bool __button_isr_initialized = false;
static SemaphoreHandle_t __button_isr_initialization_mutex = xSemaphoreCreateMutex();

Button::Button(thermostat_component_id_t id, gpio_num_t pin, uint8_t pressedState, pButtonHandler handler)
    : Component(id),
      ISRServiceable(__button_isr_event_queue),
      _pin(pin),
      _pressedState(pressedState),
      _handler(handler)
{
  __init_buttons();

  gpio_int_type_t interruptType = pressedState == 1 ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE;
  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_intr_type(pin, interruptType));

  gpio_pull_mode_t pullMode = pressedState == 1 ? GPIO_PULLDOWN_ONLY : GPIO_PULLUP_ONLY;
  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_pull_mode(pin, pullMode));

  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_isr_handler_add(pin, (void (*)(void *))ButtonISRHandlers::ButtonIsr, (void *)this));
}

Button::~Button()
{
  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_isr_handler_remove(_pin));
}

void Button::replaceHandler(pButtonHandler handler)
{
  _handler = handler;
}

void Button::__init_buttons()
{
  if (!__button_isr_initialized)
  {
    xSemaphoreTake(__button_isr_initialization_mutex, portMAX_DELAY);
    if (!__button_isr_initialized)
    {
      ESP_LOGI(BUTTON_TAG, "initializing");
      __button_container_semaphore = xSemaphoreCreateMutex();
      __button_isr_event_queue = xQueueCreate(10, sizeof(Button *));
      xTaskCreate(ButtonISRHandlers::ButtonActionHandler, "ButtonActionHandler", 2048, NULL, 10, NULL);
      ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_install_isr_service(0));
      __button_isr_initialized = true;
      ESP_LOGI(BUTTON_TAG, "initialized");
    }
    xSemaphoreGive(__button_isr_initialization_mutex);
  }
}
