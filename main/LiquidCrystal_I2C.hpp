#pragma once

#include "Component.hpp"
#include "Print.h"
#include "I2C.hpp"

#define LCD_5x8DOTS 0x00

class LiquidCrystal_I2C : public Print, private I2C
{
public:
    LiquidCrystal_I2C(const uint8_t addr, const uint8_t rows, const uint8_t cols, const uint8_t charsize = LCD_5x8DOTS);
    ~LiquidCrystal_I2C();
    // void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void clear() const;
    void home() const;
    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft() const;
    void scrollDisplayRight() const;
    void printLeft();
    void printRight();
    void leftToRight();
    void rightToLeft();
    void shiftIncrement();
    void shiftDecrement();
    void noBacklight();
    void backlight();
    void autoscroll();
    void noAutoscroll();
    void createChar(const uint8_t location, const uint8_t charMap[]);
    void setCursor(const uint8_t row, const uint8_t column) const;

    void command(const uint8_t value) const;

    void printstr(const char[]);

    // Print implementations
    virtual size_t write(const uint8_t value);
    virtual size_t write(const uint8_t *buffer, size_t size);

    ////Unsupported API functions (not implemented in this library)
    uint8_t status();
    void setContrast(uint8_t new_val);
    uint8_t keypad();
    void setDelay(int, int);
    void on();
    void off();
    uint8_t init_bargraph(uint8_t graphtype);
    void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end);
    void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end);

private:
    void init();
    void send(const uint8_t value, const uint8_t mode) const;
    void write4bits(const uint8_t value) const;
    void expanderWrite(const uint8_t data) const;
    void pulseEnable(const uint8_t data) const;

    static void delayUs(const uint32_t delay);

private:
    const uint8_t _rows;
    const uint8_t _cols;
    const uint8_t _charSize;

    uint8_t _displayFunction;
    uint8_t _displayControl;
    uint8_t _displayMode;
    uint8_t _numLines;
    uint8_t _backlightVal;
};
