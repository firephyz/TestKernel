#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include <stdint.h>
#include "kinit.h"

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define CONSOLE_SIZE CONSOLE_HEIGHT * CONSOLE_WIDTH

// Console state flags
#define CONSOLE_NORMAL			0x00
#define CONSOLE_IS_SCROLLING 	0x01
#define CONSOLE_FREEZE_CURSOR 	0x02
#define CONSOLE_IS_RESETING		0x04

#define CONSOLE_TAB_SIZE	4

#define PRT_BASE_2		2
#define PRT_BASE_8		8
#define PRT_BASE_10		10
#define PRT_BASE_16		16

#define VGA_COLOR(fg, bg) fg | (bg << 4)
#define SET_VGA_ENTRY(character) stdout.color << 8 | character

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

	// Max characters displayed could actually be less than CONSOLE_SIZE
	// because of special characters like tab.
	int char_index;
	char char_buffer[CONSOLE_SIZE];

	char * prompt_string;
	int line_index; // Used to identify when the input line runs off the edge of the screen

	int state;
};

void console_init();
void console_clear_screen();
void console_print_string(char * string);
void console_print_int(int32_t num, int base);
void console_putchar(char character);
void console_check_bounds();

static inline void console_move_cursor(int x, int y) {

	int index = y * CONSOLE_WIDTH + x;

	outb(VGA_ADDRESS_PORT, VGA_CURSOR_LOC_LOW);
	outb(VGA_DATA_PORT, (uint8_t)(index & 0xFF));
	outb(VGA_ADDRESS_PORT, VGA_CURSOR_LOC_HIGH);
	outb(VGA_DATA_PORT, (uint8_t)((index >> 8) & 0xFF));
}

#endif