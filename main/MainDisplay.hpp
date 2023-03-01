#pragma once

#include "Display.hpp"


class MainDisplay: public Display{
    private:
        rectangle rectCurrentTemp = {0,0,0,0};
        rectangle rectSetPoint = {0,0,0,0};
        float lastTemp = 0.0;
        int lastSetpoint = 0;

    public:
        MainDisplay();
        ~MainDisplay();

        virtual void makeActive();
        virtual void makeInactive();

        void writeTemp(const float temp);

        void writeSetPoint(const int setpoint);
};
