#ifndef LCD_SPI_GPIO_DRIVER_H
#define LCD_SPI_GPIO_DRIVER_H

#include "Driver_SPI.h"
#include "Driver_GPIO.h"
#include "LCD_Driver.h"

typedef struct {
    LCD_Driver       lcd_driver;
    ARM_DRIVER_SPI  *spi;
    ARM_DRIVER_GPIO *gpio;
    uint8_t          latch_pin;
} LCD_Driver_SPI_GPIO;

void lcd_init_spi_gpio(LCD_Driver_SPI_GPIO *driver, ARM_DRIVER_SPI *spi, ARM_DRIVER_GPIO *gpio, uint8_t latch_pin);

#ifdef LCD_SPI_GPIO_DRIVER_IMPLEMENTATION

static void spi_gpio_sr_latch_pulse(LCD_Driver_SPI_GPIO *driver) {
    Driver_GPIO0.SetOutput(driver->latch_pin, 0U); // ensure low
    for (volatile int i = 0; i < 150; i++) { __NOP(); }

    Driver_GPIO0.SetOutput(driver->latch_pin, 1U); // rising edge latches
    for (volatile int i = 0; i < 150; i++) { __NOP(); }

    Driver_GPIO0.SetOutput(driver->latch_pin, 0U); // back to idle low
    for (volatile int i = 0; i < 150; i++) { __NOP(); }
}

static void spi_gpio_sr_write(LCD_Driver_SPI_GPIO *driver, uint8_t v) {
    driver->spi->Send(&v, 1U);
    while (driver->spi->GetStatus().busy) { }
    spi_gpio_sr_latch_pulse(driver);
}

static void spi_enable_pulse(LCD_Driver_SPI_GPIO *driver, uint8_t data) {
    spi_gpio_sr_write(driver, data & (uint8_t)~SR_E);
    spi_gpio_sr_write(driver, data | SR_E);
    spi_gpio_sr_write(driver, data & (uint8_t)~SR_E);
}

int lcd_spi_gpio_driver_proc(LCD_Driver *driver, LCD_Driver_Proc_Op op, uint8_t data) {
    LCD_Driver_SPI_GPIO *driver_spi_gpio = (LCD_Driver_SPI_GPIO *)driver;

    switch (op) {
    case LCD_DRIVER_PROC_OP_ENABLE_PULSE: spi_enable_pulse(driver_spi_gpio, data); break;
    }
    return 0;
}

void lcd_init_spi_gpio(LCD_Driver_SPI_GPIO *driver, ARM_DRIVER_SPI *spi, ARM_DRIVER_GPIO *gpio, uint8_t latch_pin) {
    driver->lcd_driver.procedure = &lcd_spi_gpio_driver_proc;
    driver->spi       = spi;
    driver->gpio      = gpio;
    driver->latch_pin = latch_pin;
}

#endif // LCD_SPI_GPIO_DRIVER_IMPLEMENTATION

#endif // LCD_SPI_GPIO_DRIVER_H
