# Generic LCD Display driver

Basic generic LCD display driver implemention.

- Inspired by the [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C) (this generic driver is not restricted to I2C).
- Header only (STB like headers).

```c
// generic driver + implementation
#define LCD_DRIVER_IMPLEMENTATION
#include "LCD_Driver.h"

// ARM CMSIS I2C implementation (see examples)
#define LCD_I2C_DRIVER_IMPLEMENTATION
#include "LCD_I2C_Driver.h"

void use_lcd() {
    // init driver implementation
    LCD_Driver_I2C i2c_lcd;
    lcd_init_i2c(&i2c_lcd, &Driver_I2C3, LCD_DISPLAY_I2C_ADDRESS);

    // use the main driver
    LCD_Driver *lcd = &i2c_lcd.lcd_driver;
    lcd_start(lcd,
              LCD_FUNCTION_SET_FLAG_4BITMODE | LCD_FUNCTION_SET_FLAG_2LINE | LCD_FUNCTION_SET_FLAG_5x8DOTS,
              LCD_CONTROL_FLAG_DISPLAY_ON | LCD_CONTROL_FLAG_CURSOR_OFF | LCD_CONTROL_FLAG_BLINK_OFF,
              LCD_ENTRY_MODE_FLAG_ENTRY_LEFT | LCD_ENTRY_MODE_FLAG_ENTRY_SHIFT_DECREMENT);
    lcd_set_cursor(lcd, 0, 0);
    lcd_print_string(lcd, "Hello, World!");
    lcd_set_cursor(lcd, 1, 0);
    lcd_print_string(lcd, ":D");
}
```
