#include "MainDisplayMode.hpp"
#include "Fonts/FreeSerifBoldItalic18pt7b.h"
#include "Fonts/FreeSansOblique12pt7b.h"

#define BACKGROUND_COLOR WHITE

#define TEMP_COLOR GREEN
#define TEMP_POSITION_X 30
#define TEMP_POSITION_Y 80
#define TEMP_FONT FreeSerifBoldItalic18pt7b

#define SETPOINT_COLOR BLUE
#define SETPOINT_POSITION_X 90
#define SETPOINT_POSITION_Y 30
#define SETPOINT_FONT FreeSansOblique12pt7b

static const char *TAG = "MainDisplayMode";

MainDisplayMode::MainDisplayMode()
    : DisplayMode(MAIN_DISPLAY_MODE)
{
}

MainDisplayMode::~MainDisplayMode() {}

void MainDisplayMode::writeTemp(const float temp)
{
    lastTemp = temp;
    ACQUIRE_ACTIVE_SCREEN
    prepForPrint(TEMP_POSITION_X, TEMP_POSITION_Y, &TEMP_FONT, TEMP_COLOR);
    fillRect(rectCurrentTemp, BACKGROUND_COLOR);
    char buffer[4] = {0};
    sprintf(buffer, "%.1f", temp);
    rectCurrentTemp = getNewBounds(buffer, TEMP_POSITION_X, TEMP_POSITION_Y);
    getOled()->printf(buffer);
    RELEASE_ACTIVE_SCREEN
}

void MainDisplayMode::writeSetPoint(const int setpoint)
{
    lastSetpoint = setpoint;
    ACQUIRE_ACTIVE_SCREEN
    prepForPrint(SETPOINT_POSITION_X, SETPOINT_POSITION_Y, &SETPOINT_FONT, SETPOINT_COLOR);
    fillRect(rectSetPoint, BACKGROUND_COLOR);
    char buffer[4] = {0};
    sprintf(buffer, "%d", setpoint);
    rectSetPoint = getNewBounds(buffer, SETPOINT_POSITION_X, SETPOINT_POSITION_Y);
    getOled()->printf(buffer);
    RELEASE_ACTIVE_SCREEN
}
