/*
 * LCD Driver ARM CMSIS I2C usage.
 */

#include "cmsis_os2.h"
#include "Driver_I2C.h"
#include "I2C_STM32.h"
#include "stm32f4xx_hal.h"

// Generic LCD driver implementation
#define LCD_DRIVER_IMPLEMENTATION
#include "Components/LCD_Driver.h"

// I2C LCD driver implementation
#define LCD_I2C_DRIVER_IMPLEMENTATION
#include "Components/LCD_I2C_Driver.h"

extern I2C_HandleTypeDef hi2c3;
extern ARM_DRIVER_I2C  Driver_I2C3;

#define LCD_DISPLAY_I2C_ADDRESS 0x27 // i2c address of the device

static void app_thread(void *argument) {
    (void)argument;

    // init I2C3
    Driver_I2C3.Initialize(NULL);
    Driver_I2C3.PowerControl(ARM_POWER_FULL);
    Driver_I2C3.Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    Driver_I2C3.Control(ARM_I2C_BUS_CLEAR, 0);

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

    for (;;) { osDelay(1000); }
}

int launch_app(void) {
    osKernelInitialize();
    osThreadNew(app_thread, NULL, NULL);
    osKernelStart();
    for (;;) {}
}
