#pragma once

#include "DisplayMode.hpp"

class MainDisplayMode : public DisplayMode
{
private:
    rectangle rectCurrentTemp = {0, 0, 0, 0};
    rectangle rectSetPoint = {0, 0, 0, 0};
    float lastTemp = 0.0;
    int lastSetpoint = 0;

public:
    MainDisplayMode();
    ~MainDisplayMode();

    void writeTemp(const float temp);

    void writeSetPoint(const int setpoint);
};
