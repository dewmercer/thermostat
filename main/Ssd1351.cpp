#include "Ssd1351.hpp"

#define DISPLAY_ENHANCEMENT (uint8_t)0xB2
#define ENHANCE_DIAPLAY (uint8_t)0xA4

void Ssd1351::sleep() 
{
    sendCommand(SSD1351_CMD_DISPLAYOFF);
}

void Ssd1351::wakeup() 
{
    uint8_t displayEnhancement[3] = {ENHANCE_DIAPLAY, 0x00, 0x00};
    sendCommand(SSD1351_CMD_DISPLAYENHANCE, displayEnhancement, sizeof(displayEnhancement));
    sendCommand(SSD1351_CMD_DISPLAYON);
}