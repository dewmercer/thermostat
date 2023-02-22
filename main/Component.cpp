#include "Component.hpp"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t __component_registry_semaphore = xSemaphoreCreateMutex();
static std::map<const thermostat_component_id_t, Component *> *__component_registry;
static bool __component_registry_initialized = false;

static void __init_component()
{
    if (!__component_registry_initialized)
    {
        xSemaphoreTake(__component_registry_semaphore, portMAX_DELAY);
        if (!__component_registry_initialized)
        {
            __component_registry = new std::map<const thermostat_component_id_t, Component *>();
            __component_registry_initialized = true;
        }
        xSemaphoreGive(__component_registry_semaphore);
    }
}

Component::Component(thermostat_component_id_t id) : _id(id)
{
    __init_component();
    Component::add(id, this);
}

Component::~Component()
{
    Component::remove(_id);
}

thermostat_component_id_t Component::id() const
{
    return _id;
}

bool Component::registered(thermostat_component_id_t id)
{
    return __component_registry->find(id) != __component_registry->end();
}

Component *Component::get(thermostat_component_id_t id)
{
    if (Component::registered(id))
    {
        return __component_registry->find(id)->second;
    }
    else
    {
        return nullptr;
    }
}

bool Component::add(thermostat_component_id_t id, Component *component)
{
    xSemaphoreTake(__component_registry_semaphore, portMAX_DELAY);
    if (Component::registered(id))
    {
        return false;
    }
    __component_registry->emplace(id, component);
    xSemaphoreGive(__component_registry_semaphore);
    return true;
}

void Component::remove(thermostat_component_id_t id)
{
    xSemaphoreTake(__component_registry_semaphore, portMAX_DELAY);
    __component_registry->erase(id);
    xSemaphoreGive(__component_registry_semaphore);
}