# ARM CMSIS SPI+GPIO example

- This driver expects users to store transmitted bytes into a shift register
  wired to the pins of an LCD display (4-bit mode). SPI is used to transfer the
  data from the board to the register and GPIO is used to control the latch pin
  on the register.
- `LCD_SPI_GPIO_Driver.h` contains the SPI+GPIO LCD driver implementation.
  - It also comes under the form of an STB like header.
  - Requires CMSIS headers (usually generated using keil studio).
- `main.c` includes both the generic driver and the SPI+GPIO implementation,
  initialize SPI and GPIO on a nucleo-64 STM32Fxx board and uses the LCD
  display.
