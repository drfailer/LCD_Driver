# ARM CMSIS I2C example

- `LCD_I2C_Driver.h` contains the I2C LCD driver implementation.
  - It also comes under the form of an STB like header.
  - Requires CMSIS headers (usually generated using keil studio).
  - The initilization function allows configuring the underlying I2C
    driver and the device address.
- `main.c` includes both the generic driver and the I2C implementation,
  initialize I2C3 on a nucleo-64 STM32Fxx board and uses the LCD display.
