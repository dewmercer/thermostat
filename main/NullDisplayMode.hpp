#pragma once

#include "DisplayMode.hpp"

class NullDisplayMode : public DisplayMode
{
public:
    NullDisplayMode()
        : DisplayMode(NULL_DISPLAY_MODE){};

    ~NullDisplayMode(){};

    virtual void makeActive(){isActive = true;};
};