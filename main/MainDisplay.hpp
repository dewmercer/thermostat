#pragma once

#include "Display.hpp"


class MainDisplay: public Display{

    public:
        MainDisplay();
        ~MainDisplay();

        virtual void makeActive();
        virtual void makeInactive();

        void writeTemp(const float temp) const;
};
