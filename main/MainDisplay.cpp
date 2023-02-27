#include "MainDisplay.hpp"
#include "Fonts/FreeSerifBoldItalic18pt7b.h"

MainDisplay::MainDisplay()
    : Display(MAIN_DISPLAY) {}

MainDisplay::~MainDisplay() {}
void MainDisplay::makeActive()
{
    Display::makeActive();
}
void MainDisplay::makeInactive()
{
    Display::makeInactive();
}

#define TEMP_POSITION_X 30
#define TEMP_POSITION_Y 30

int16_t txtBoundsX = 0, txtBoundsY = 0;
uint16_t txtBoundsW = 0, txtBoundsH = 0;



void MainDisplay::writeTemp(const float temp) const
{
    ACQUIRE_ACTIVE_SCREEN
    getDisplay()->setTextColor(GREEN);
    getDisplay()->setFont(&FreeSerifBoldItalic18pt7b);
    getDisplay()->setCursor(TEMP_POSITION_X, TEMP_POSITION_Y);
    getDisplay()->fillRect(TEMP_POSITION_X, TEMP_POSITION_Y - txtBoundsH + 2, 127 - TEMP_POSITION_X, txtBoundsH, WHITE);
    char buffer[4] = {0};
    sprintf(buffer, "%.1f", temp);
    getDisplay()->getTextBounds(buffer, TEMP_POSITION_X, TEMP_POSITION_Y, &txtBoundsX, &txtBoundsY, &txtBoundsW, &txtBoundsH);
    getDisplay()->printf(buffer);
    RELEASE_ACTIVE_SCREEN
}
