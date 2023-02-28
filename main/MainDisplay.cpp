#include "MainDisplay.hpp"
#include "Fonts/FreeSerifBoldItalic18pt7b.h"

MainDisplay::MainDisplay()
    : Display(MAIN_DISPLAY)
     {}

MainDisplay::~MainDisplay() {}
void MainDisplay::makeActive()
{
    Display::makeActive();
}
void MainDisplay::makeInactive()
{
    Display::makeInactive();
}

#define BACKGROUND_COLOR WHITE
#define TEMP_POSITION_X 30
#define TEMP_POSITION_Y 80

void MainDisplay::writeTemp(const float temp) 
{
    ACQUIRE_ACTIVE_SCREEN
    getDisplay()->setTextColor(GREEN);
    getDisplay()->setFont(&FreeSerifBoldItalic18pt7b);
    getDisplay()->setCursor(TEMP_POSITION_X, TEMP_POSITION_Y);
    fillRect(rectCurrentTemp, BACKGROUND_COLOR);
    getDisplay()->fillRect(rectCurrentTemp.x, rectCurrentTemp.y, rectCurrentTemp.w, rectCurrentTemp.y, BACKGROUND_COLOR);
    char buffer[4] = {0};
    sprintf(buffer, "%.1f", temp);
    rectCurrentTemp = getNewBounds(buffer, TEMP_POSITION_X, TEMP_POSITION_Y);
    getDisplay()->printf(buffer);
    RELEASE_ACTIVE_SCREEN
}
