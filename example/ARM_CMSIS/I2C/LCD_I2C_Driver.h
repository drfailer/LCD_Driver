/*
 * LCD Driver ARM CMSIS I2C implementation.
 */

#ifndef LCD_I2C_DRIVER_H
#define LCD_I2C_DRIVER_H

#include "cmsis_os2.h"
#include "Driver_I2C.h"
#include "LCD_Driver.h"

typedef struct {
    LCD_Driver      lcd_driver;
    ARM_DRIVER_I2C *i2c;
    uint8_t         address;
} LCD_Driver_I2C;

void lcd_init_i2c(LCD_Driver_I2C *driver, ARM_DRIVER_I2C *I2C_Driver, uint8_t address);

#ifdef LCD_I2C_DRIVER_IMPLEMENTATION

static void i2c_enable_pulse(LCD_Driver_I2C *driver, uint8_t data) {
    uint8_t buf[3];

    buf[0] = data & ~SR_E; // Ensure Enable is LOW before pulsing
    buf[1] = data | SR_E;  // Bring Enable HIGH (latches data into LCD)
    buf[2] = data & ~SR_E; // Bring Enable LOW again

    driver->i2c->MasterTransmit(driver->address, buf, 3, false);
    while (driver->i2c->GetStatus().busy) { }
}

int lcd_i2c_driver_proc(LCD_Driver *driver, LCD_Driver_Proc_Op op, uint8_t data) {
    LCD_Driver_I2C *driver_i2c = (LCD_Driver_I2C *)driver;

    switch (op) {
    case LCD_DRIVER_PROC_OP_ENABLE_PULSE: i2c_enable_pulse(driver_i2c, data); break;
    }
    return 0;
}

void lcd_init_i2c(LCD_Driver_I2C *driver, ARM_DRIVER_I2C *I2C_Driver, uint8_t address) {
    driver->lcd_driver.procedure = &lcd_i2c_driver_proc;
    driver->address = address;
    driver->i2c    = I2C_Driver;
}

#endif // LCD_I2C_DRIVER_IMPLEMENTATION

#endif // LCD_I2C_DRIVER_H
