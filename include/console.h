#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include <stdint.h>
#include "kernel.h"

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25

#define PRT_BASE_2		2
#define PRT_BASE_8		8
#define PRT_BASE_10		10
#define PRT_BASE_16		16

#define VGA_COLOR(fg, bg) fg | (bg << 4)
#define SET_VGA_ENTRY(character) system_out.color << 8 | character

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
	int8_t x_pos;
	int8_t y_pos;
	uint8_t color;
	uint16_t * buffer;
};

void console_init();
void console_clear_screen();
void console_print_string(char * string);
void console_print_int(int32_t num, int base);
void console_putchar(char character);

static inline void console_move_cursor(int x, int y) {

	int index = y * CONSOLE_WIDTH + x;

	outb(VGA_ADDRESS_PORT, VGA_CURSOR_LOC_LOW);
	outb(VGA_DATA_PORT, (uint8_t)(index & 0xFF));
	outb(VGA_ADDRESS_PORT, VGA_CURSOR_LOC_HIGH);
	outb(VGA_DATA_PORT, (uint8_t)((index >> 8) & 0xFF));
}

// Handle cursor updates
static inline void console_check_bounds() {
	if(system_out.x_pos == CONSOLE_WIDTH) {
		system_out.x_pos = 0;
		++system_out.y_pos;
	}

	if(system_out.y_pos == CONSOLE_HEIGHT) {
		system_out.y_pos = 0;
	}
}

#endif