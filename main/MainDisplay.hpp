#pragma once

#include "Display.hpp"


class MainDisplay: public Display{
    private:
        rectangle rectCurrentTemp;
        rectangle rectSetPoint;

        public:
            MainDisplay();
            ~MainDisplay();

            virtual void makeActive();
            virtual void makeInactive();

            void writeTemp(const float temp);

            void writeSetPoint(const int setpoint);
};
