#include "LiquidCrystal_I2C.hpp"

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include <rom/ets_sys.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100 // Enable bit
#define Rw 0b00000010 // Read/Write bit
#define Rs 0b00000001 // Register select bit


static bool __lcd_initialized = false;
static SemaphoreHandle_t __lcd_initialization_mutex = xSemaphoreCreateMutex();
static const char *LCD_TAG = "LCD";

static void __init_lcd()
{
    if (!__lcd_initialized)
    {
        xSemaphoreTake(__lcd_initialization_mutex, portMAX_DELAY);
        if (!__lcd_initialized)
        {
            ESP_LOGI(LCD_TAG, "__init_lcd initializing");
            __lcd_initialized = true;
        }
        ESP_LOGI(LCD_TAG, "__init_lcd initialized");
        xSemaphoreGive(__lcd_initialization_mutex);
    }
}

LiquidCrystal_I2C::LiquidCrystal_I2C(const uint8_t addr, const uint8_t rows, const uint8_t cols, const uint8_t charSize)
    : I2C(DISPLAY_LCD, addr),
      _rows(rows),
      _cols(cols),
      _charSize(charSize),
      _backlightVal(LCD_NOBACKLIGHT)
{
    ESP_LOGI(LCD_TAG, "ENTER: ctor");
    init();
    ESP_LOGI(LCD_TAG, "EXIT: ctor");
};

LiquidCrystal_I2C::~LiquidCrystal_I2C() {}

void LiquidCrystal_I2C::init()
{
    ESP_LOGI(LCD_TAG, "ENTER: init");
    __init_lcd();
    _displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    if (_rows > 1)
    {
        _displayFunction |= LCD_2LINE;
    }
    _numLines = _rows;

    // for some 1 line displays you can select a 10 pixel high font
    if ((_charSize != 0) && (_rows == 1))
    {
        _displayFunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delayUs(50000);

    // Now we pull both RS and R/W low to begin commands
    expanderWrite(_backlightVal); // reset expanderand turn backlight off (Bit 8 =1)
    delayUs(1000000);

    // put the LCD into 4 bit mode
    //  this is according to the hitachi HD44780 datasheet
    //  figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03 << 4);
    delayUs(4500); // wait min 4.1ms

    // second try
    write4bits(0x03 << 4);
    delayUs(4500); // wait min 4.1ms

    // third go!
    write4bits(0x03 << 4);
    delayUs(150);

    // finally, set to 4-bit interface
    write4bits(0x02 << 4);

    // set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayFunction);

    // turn the display on with no cursor or blinking default
    _displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for roman languages)
    _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    command(LCD_ENTRYMODESET | _displayMode);

    home();
    ESP_LOGI(LCD_TAG, "EXIT: init");
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C::clear() const
{
    command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
    delayUs(2000);             // this command takes a long time!
}

void LiquidCrystal_I2C::home() const
{
    command(LCD_RETURNHOME); // set cursor position to zero
    delayUs(2000);           // this command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t row, uint8_t col) const
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > _numLines)
    {
        row = _numLines - 1; // we count rows starting w/0
    }
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay()
{
    _displayControl &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}
void LiquidCrystal_I2C::display()
{
    _displayControl |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor()
{
    _displayControl &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}
void LiquidCrystal_I2C::cursor()
{
    _displayControl |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink()
{
    _displayControl &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}
void LiquidCrystal_I2C::blink()
{
    _displayControl |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) const
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C::scrollDisplayRight(void) const
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void)
{
    _displayMode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void)
{
    _displayMode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void)
{
    _displayMode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void)
{
    _displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters

void LiquidCrystal_I2C::createChar(const uint8_t location, const uint8_t charmap[])
{
    uint8_t loc = location & 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (loc << 3));
    for (int i = 0; i < 8; i++)
    {
        write(charmap[i]);
    }
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::noBacklight(void)
{
    ESP_LOGI(LCD_TAG, "ENTER: noBacklight");
    _backlightVal = LCD_NOBACKLIGHT;
    expanderWrite(0);
    ESP_LOGI(LCD_TAG, "EXIT: noBacklight");
}

void LiquidCrystal_I2C::backlight(void)
{
    ESP_LOGI(LCD_TAG, "ENTER: backlight");
    _backlightVal = LCD_BACKLIGHT;
    expanderWrite(0);
    ESP_LOGI(LCD_TAG, "EXIT: backlight");
}

/*********** mid level commands, for sending data/cmds */

void LiquidCrystal_I2C::command(const uint8_t value) const
{
    send(value, 0);
}

/************ low level data pushing commands **********/

void LiquidCrystal_I2C::expanderWrite(const uint8_t data) const
{
    I2C::write(data | _backlightVal);
}

void LiquidCrystal_I2C::write4bits(const uint8_t data) const
{
    expanderWrite(data);
    pulseEnable(data);
}

void LiquidCrystal_I2C::pulseEnable(const uint8_t data) const
{
    expanderWrite(data | En); // En high
    delayUs(1);               // enable pulse must be >450ns

    expanderWrite(data & ~En); // En low
    delayUs(50);               // commands need > 37us to settle
}

inline void LiquidCrystal_I2C::delayUs(const uint32_t delay)
{
    ets_delay_us(delay);
}

// write either command or data
void LiquidCrystal_I2C::send(const uint8_t value, const uint8_t mode) const
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write4bits((highnib) | mode);
    write4bits((lownib) | mode);
}

inline size_t LiquidCrystal_I2C::write(const uint8_t value)
{
    send(value, Rs);
    return 1;
}

size_t LiquidCrystal_I2C::write(const uint8_t *buffer, const size_t size)
{
    size_t sz = size;
    size_t n = 0;
    while (sz--)
    {
        n += write(*buffer++);
    }
    return n;
}

// unsupported API functions
void LiquidCrystal_I2C::off() {}
void LiquidCrystal_I2C::on() {}
void LiquidCrystal_I2C::setDelay(int cmdDelay, int charDelay) {}
uint8_t LiquidCrystal_I2C::status() { return 0; }
uint8_t LiquidCrystal_I2C::keypad() { return 0; }
uint8_t LiquidCrystal_I2C::init_bargraph(uint8_t graphtype) { return 0; }
void LiquidCrystal_I2C::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end) {}
void LiquidCrystal_I2C::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_row_end) {}
void LiquidCrystal_I2C::setContrast(uint8_t new_val) {}
