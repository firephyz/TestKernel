#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25

#define VGA_COLOR(fg, bg) fg | (bg << 4)

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

struct console {
	uint8_t x_pos;
	uint8_t y_pos;
	uint8_t color;
	uint16_t * buffer;
};

void console_init();
void console_clear_screen();
void console_putchar(int x, int y, char character);
static inline void console_check_bounds();
static inline uint16_t set_vga_entry(char character);
size_t strlen(char * string);
void console_write_string(char * string);
void console_write_character(char character);

#endif