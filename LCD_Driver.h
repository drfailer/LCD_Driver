/*
 * LCD Display generic driver.
 *
 * - This is an STB like header only library, one must define LCD_DRIVER_IMPLEMENTATION
 *   before including this file to generate the function implementation.
 * - It is a generic "abstract" driver, implementations should provide the
 *   LCD_Driver_Proc implementation (see the ARM CMSIS I2C driver example).
 */

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>

//
// Flags for display entry mode.
//
typedef enum {
    LCD_ENTRY_MODE_FLAG_ENTRY_RIGHT           = 0x00,
    LCD_ENTRY_MODE_FLAG_ENTRY_LEFT            = 0x02,
    LCD_ENTRY_MODE_FLAG_ENTRY_SHIFT_INCREMENT = 0x01,
    LCD_ENTRY_MODE_FLAG_ENTRY_SHIFT_DECREMENT = 0x00,
} LCD_Entry_Mode_Flag;

//
// Flags for display on/off control.
//
typedef enum {
    LCD_CONTROL_FLAG_DISPLAY_ON  = 0x04,
    LCD_CONTROL_FLAG_DISPLAY_OFF = 0x00,
    LCD_CONTROL_FLAG_CURSOR_ON   = 0x02,
    LCD_CONTROL_FLAG_CURSOR_OFF  = 0x00,
    LCD_CONTROL_FLAG_BLINK_ON    = 0x01,
    LCD_CONTROL_FLAG_BLINK_OFF   = 0x00,
} LCD_Control_Flag;

//
// Flags for display/cursor shift.
//
typedef enum {
    LCD_CURSOR_SHIFT_FLAG_DISPLAY_MOVE = 0x08,
    LCD_CURSOR_SHIFT_FLAG_CURSOR_MOVE  = 0x00,
    LCD_CURSOR_SHIFT_FLAG_MOVE_RIGHT   = 0x04,
    LCD_CURSOR_SHIFT_FLAG_MOVE_LEFT    = 0x00,
} LCD_Cursor_Shift_Flag;


//
// Flags for function set.
//
typedef enum {
    LCD_FUNCTION_SET_FLAG_8BITMODE = 0x10,
    LCD_FUNCTION_SET_FLAG_4BITMODE = 0x00,
    LCD_FUNCTION_SET_FLAG_2LINE    = 0x08,
    LCD_FUNCTION_SET_FLAG_1LINE    = 0x00,
    LCD_FUNCTION_SET_FLAG_5x10DOTS = 0x04,
    LCD_FUNCTION_SET_FLAG_5x8DOTS  = 0x00,
} LCD_Function_Set_Flag;

//
// Flags for backlight control.
//
typedef enum {
    LCD_BACKLIGHT_CONTROL_FLAG_BACKLIGHT   = 0x08,
    LCD_BACKLIGHT_CONTROL_FLAG_NO_BACKLIGHT = 0x00,
} LCD_Backlight_Control_Flag;

//
// LCD Display commands.
//
typedef enum {
    LCD_COMMAND_CLEAR_DISPLAY   = 0x01,
    LCD_COMMAND_RETUR_NHOME     = 0x02,
    LCD_COMMAND_ENTRY_MODE_SET  = 0x04,
    LCD_COMMAND_DISPLAY_CONTROL = 0x08,
    LCD_COMMAND_CURSOR_SHIFT    = 0x10,
    LCD_COMMAND_FUNCTION_SET    = 0x20,
    LCD_COMMAND_SET_CGRAM_ADDR  = 0x40,
    LCD_COMMAND_SET_DDRAM_ADDR  = 0x80,
} LCD_Command;

typedef struct LCD_Driver LCD_Driver;

//
// LCD Driver proc: we use a single function pointer with an operation
// parameter for the driver.
//
typedef enum {
    LCD_DRIVER_PROC_OP_ENABLE_PULSE,
} LCD_Driver_Proc_Op;
typedef int (*LCD_Driver_Proc)(LCD_Driver *driver, LCD_Driver_Proc_Op op, uint8_t data);

//
// LCD Driver base.
//
struct LCD_Driver {
    LCD_Driver_Proc procedure;
    uint8_t backlight_flags;
    uint8_t function_set_flags;
    uint8_t display_control_flags;
    uint8_t entry_mode_flags;
};

//
// LCD Display API.
//

void lcd_start(LCD_Driver *driver, uint8_t function_set_flags, uint8_t display_control_flags, uint8_t entry_mode_flags);
void lcd_command(LCD_Driver *driver, uint8_t cmd);

void lcd_set_cursor(LCD_Driver *driver, uint8_t row, uint8_t col);

void lcd_print(LCD_Driver *driver, const char *str, size_t size);
void lcd_print_string(LCD_Driver *driver, const char *str);

#define lcd_clear(driver)        lcd_command((driver), LCD_COMMAND_CLEAR_DISPLAY)
#define lcd_home(driver)         lcd_command((driver), LCD_COMMAND_RETUR_NHOME)
#define lcd_scroll_left(driver)  lcd_command((driver), LCD_COMMAND_CURSOR_SHIFT | LCD_CURSOR_SHIFT_FLAG_DISPLAY_MOVE | LCD_CURSOR_SHIFT_FLAG_MOVE_LEFT)
#define lcd_scroll_right(driver) lcd_command((driver), LCD_COMMAND_CURSOR_SHIFT | LCD_CURSOR_SHIFT_FLAG_DISPLAY_MOVE | LCD_CURSOR_SHIFT_FLAG_MOVE_RIGHT)

// flag related functions are generated using an X-macro
#define GENERATE_LCD_FLAG_FUNCTION_DECLARATION(suffix)    \
    void lcd_flags_add_##suffix (LCD_Driver *driver, uint8_t flags);   \
    void lcd_flags_remove_##suffix (LCD_Driver *driver, uint8_t flags);
GENERATE_LCD_FLAG_FUNCTION_DECLARATION(function_set)
GENERATE_LCD_FLAG_FUNCTION_DECLARATION(display_control)
GENERATE_LCD_FLAG_FUNCTION_DECLARATION(entry_mode)
#undef GENERATE_LCD_FLAG_FUNCTION_DECLARATION

#define lcd_display_on(driver)  lcd_flags_add_display_control((driver), LCD_CONTROL_FLAG_DISPLAY_ON)
#define lcd_display_off(driver) lcd_flags_remove_display_control((driver), LCD_CONTROL_FLAG_DISPLAY_ON)

#define lcd_cursor_on(driver)  lcd_flags_add_display_control((driver), LCD_CONTROL_FLAG_CURSOR_ON)
#define lcd_cursor_off(driver) lcd_flags_remove_display_control((driver), LCD_CONTROL_FLAG_CURSOR_ON)

#define lcd_blink_on(driver)  lcd_flags_add_display_control((driver), LCD_CONTROL_FLAG_BLINK_ON)
#define lcd_blink_off(driver) lcd_flags_remove_display_control((driver), LCD_CONTROL_FLAG_BLINK_ON)

#define lcd_left_to_right_mode(driver) lcd_flags_add_entry_mode((driver), LCD_ENTRY_MODE_FLAG_ENTRY_LEFT)
#define lcd_right_to_left_mode(driver) lcd_flags_remove_entry_mode((driver), LCD_ENTRY_MODE_FLAG_ENTRY_LEFT)

#define lcd_autoscroll_on(driver)  lcd_flags_add_entry_mode((driver), LCD_ENTRY_MODE_FLAG_ENTRY_SHIFT_INCREMENT)
#define lcd_autoscroll_off(driver) lcd_flags_remove_entry_mode((driver), LCD_ENTRY_MODE_FLAG_ENTRY_SHIFT_INCREMENT)

/******************************************************************************/
/*                               Implementation                               */
/******************************************************************************/

#ifdef LCD_DRIVER_IMPLEMENTATION

#include <string.h>

//
// Control bits.
//
#define SR_RS         (1u << 0)       // Register Select
#define SR_RW         (1u << 1)       // Read/Write
#define SR_E          (1u << 2)       // Enable pulse
#define SR_BACKLIGHT  (1u << 3)       // Backlight control

// LCD Driver Procs ////////////////////////////////////////////////////////////

static void lcd_enable_pulse(LCD_Driver *driver, uint8_t data) {
    driver->procedure(driver, LCD_DRIVER_PROC_OP_ENABLE_PULSE, data);
}

// LCD Internals ///////////////////////////////////////////////////////////////

static void lcd_write_4bits(LCD_Driver *driver, uint8_t nibble, uint8_t rs) {
    uint8_t out = (nibble & 0x0f) << 4;
    if (rs) out |= SR_RS;
    lcd_enable_pulse(driver, out | driver->backlight_flags);
    osDelay(1);
}

static void lcd_send(LCD_Driver *driver, uint8_t data, uint8_t rs) {
    lcd_write_4bits(driver, data >> 4, rs);
    lcd_write_4bits(driver, data & 0x0f, rs);
}

// LCD API /////////////////////////////////////////////////////////////////////

void lcd_command(LCD_Driver *driver, uint8_t cmd) {
	lcd_send(driver, cmd, 0u);
    if (cmd == LCD_COMMAND_CLEAR_DISPLAY || cmd == LCD_COMMAND_RETUR_NHOME) {
        osDelay(2); // delay for long commands
    }
}

void lcd_set_cursor(LCD_Driver *driver, uint8_t row, uint8_t col) {
	uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    // TODO: we need to check the number of lines
	lcd_command(driver, LCD_COMMAND_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void lcd_print(LCD_Driver *driver, const char *s, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        lcd_send(driver, s[i], 1u);
    }
}

inline void lcd_print_string(LCD_Driver *driver, const char *str) {
    lcd_print(driver, str, strlen(str));
}

inline void lcd_backlight_on(LCD_Driver *driver) {
    driver->backlight_flags = LCD_BACKLIGHT_CONTROL_FLAG_BACKLIGHT;
    lcd_command(driver, 0);
}

inline void lcd_backlight_off(LCD_Driver *driver) {
    driver->backlight_flags = LCD_BACKLIGHT_CONTROL_FLAG_NO_BACKLIGHT;
    lcd_command(driver, 0);
}

// flag related functions are generated using an X-macro
#define GENERATE_LCD_FLAG_FUNCTION(suffix, flag_field, command_bit)             \
    inline void lcd_flags_add_##suffix (LCD_Driver *driver, uint8_t flags) {    \
        driver->flag_field |= flags;                                            \
        lcd_command(driver, command_bit | driver->flag_field);                  \
    }                                                                           \
    inline void lcd_flags_remove_##suffix (LCD_Driver *driver, uint8_t flags) { \
        driver->flag_field &= ~flags;                                           \
        lcd_command(driver, command_bit | driver->flag_field);                  \
    }
GENERATE_LCD_FLAG_FUNCTION(function_set, function_set_flags, LCD_COMMAND_FUNCTION_SET)
GENERATE_LCD_FLAG_FUNCTION(display_control, display_control_flags, LCD_COMMAND_DISPLAY_CONTROL)
GENERATE_LCD_FLAG_FUNCTION(entry_mode, entry_mode_flags, LCD_COMMAND_ENTRY_MODE_SET)
#undef GENERATE_LCD_FLAG_FUNCTION

void lcd_start(LCD_Driver *driver, uint8_t function_set_flags, uint8_t display_control_flags, uint8_t entry_mode_flags) {
    driver->function_set_flags = 0;
    driver->display_control_flags = 0;
    driver->entry_mode_flags = 0;
    driver->backlight_flags = LCD_BACKLIGHT_CONTROL_FLAG_BACKLIGHT;

    osDelay(50);

    // force 8-bits reset
    lcd_write_4bits(driver, 0x03u, 0u); osDelay(5);
    lcd_write_4bits(driver, 0x03u, 0u); osDelay(1);
    lcd_write_4bits(driver, 0x03u, 0u); osDelay(1);

    // set to 4-bits mode
    lcd_write_4bits(driver, 0x02u, 0u); osDelay(1);

    // display configuration
    lcd_flags_add_function_set(driver, function_set_flags);
    lcd_flags_add_display_control(driver, display_control_flags);
    lcd_command(driver, LCD_COMMAND_CLEAR_DISPLAY);
    lcd_flags_add_entry_mode(driver, entry_mode_flags);
}

#endif // LCD_DISPLAY_IMPLEMENTATION

#endif // LCD_DRIVER_H
