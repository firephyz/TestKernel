#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

struct console system_out;

void kernel_start(void) {

	console_init();

	console_write_string("Hello OS World!\n");
	console_write_string("\n");
	console_write_string(" > How are you today?\n");

	while (1) {}
}

void console_init() {

	system_out.x_pos = 0;
	system_out.y_pos = 0;
	system_out.color = VGA_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	system_out.buffer = (uint16_t *)0xB8000;

	console_clear_screen();
}

void console_write_string(char * string) {

	for(int i = 0; i < strlen(string); ++i) {
		console_write_character(string[i]);
	}
}

void console_clear_screen() {

	for(int y = 0; y < CONSOLE_HEIGHT; ++y) {
		for(int x = 0; x < CONSOLE_WIDTH; ++x) {
			console_putchar(x, y, ' ');
		}
	}
}

void console_write_character(char character) {

	console_putchar(system_out.x_pos, system_out.y_pos, character);

	if (character != '\n') ++system_out.x_pos;

	console_check_bounds();
}

void console_putchar(int x, int y, char character) {

	if(character == '\n') {
		system_out.x_pos = 0;
		system_out.y_pos += 1;

		console_check_bounds();
	}
	else {
		int index = CONSOLE_WIDTH * y + x;
		system_out.buffer[index] = set_vga_entry(character);
	}
}

size_t strlen(char * string) {

	size_t len = 0;
	while(string[len] != '\0') {
		++len;
	}

	return len;
}

static inline void console_check_bounds() {
	if(system_out.x_pos == CONSOLE_WIDTH) {
		system_out.x_pos = 0;
		++system_out.y_pos;

		if(system_out.y_pos == CONSOLE_HEIGHT) {
			system_out.y_pos = 0;
		}
	}
}

static inline uint16_t set_vga_entry(char character) {
	return system_out.color << 8 | character;
}