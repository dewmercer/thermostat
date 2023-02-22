#pragma once

#include "I2C.hpp"

class AM2320 : private I2C
{
    public:
        AM2320();
        ~AM2320();
        uint16_t temperature() const;
};
