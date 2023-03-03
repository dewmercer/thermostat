#pragma once
#include "Component.hpp"

class I2C: public Component{
    private:
        const uint8_t _addr;

    public:
        I2C(const component_id_t component, const uint8_t addr);
        ~I2C();

        void write(const uint8_t data) const;
        void write(const uint8_t *buffer, const size_t len) const;
        void writeRead(const uint8_t addredd, const uint8_t *writeBuffer, const size_t writeBufferLen, uint8_t *readBuffer, const size_t readBufferLen) const;
};
