/*
 * LCD Driver ARM CMSIS SPI+GPIO usage.
 */

#include "cmsis_os2.h"
#include "Driver_I2C.h"
#include "I2C_STM32.h"
#include "stm32f4xx_hal.h"

// Generic LCD driver implementation
#define LCD_DRIVER_IMPLEMENTATION
#include "LCD_Driver.h"

// I2C LCD driver implementation
#define LCD_I2C_DRIVER_IMPLEMENTATION
#include "LCD_SPI_GPIO_Driver.h"

extern SPI_HandleTypeDef hspi1;
extern ARM_DRIVER_SPI  Driver_SPI1;
extern ARM_DRIVER_GPIO Driver_GPIO0;

#define LCD_DISPLAY_LATCH_PIN GPIO_PIN_ID_PORTB(6)

static void app_thread(void *argument) {
    (void)argument;

    // GPIO0 init
    Driver_GPIO0.Setup(LCD_DISPLAY_LATCH_PIN, NULL);
    Driver_GPIO0.SetDirection(LCD_DISPLAY_LATCH_PIN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetOutputMode(LCD_DISPLAY_LATCH_PIN, ARM_GPIO_PUSH_PULL);
    Driver_GPIO0.SetPullResistor(LCD_DISPLAY_LATCH_PIN, ARM_GPIO_PULL_NONE);
    Driver_GPIO0.SetOutput(LCD_DISPLAY_LATCH_PIN, 0U); // idle low

    // SPI2 init (Mode 0 is what 74HC595 wants)
    Driver_SPI1.Initialize(NULL);
    Driver_SPI1.PowerControl(ARM_POWER_FULL);
    Driver_SPI1.Control(ARM_SPI_MODE_MASTER
                      | ARM_SPI_CPOL0_CPHA0
                      | ARM_SPI_DATA_BITS(8)
                      | ARM_SPI_MSB_LSB
                      | ARM_SPI_SS_MASTER_UNUSED,
                      1000000U);
    Driver_SPI1.Control(ARM_SPI_SET_DEFAULT_TX_VALUE, 0x00U);

    // init driver implementation
    LCD_Driver_SPI_GPIO spi_gpio_driver;
    lcd_init_spi_gpio(&spi_gpio_driver, &Driver_SPI1, &Driver_GPIO0, LCD_DISPLAY_LATCH_PIN);

    // use the main driver
    LCD_Driver *lcd = &spi_gpio_driver.lcd_driver;
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
