#include "console.h"
#include "kernel.h"
#include <stdint.h>

void console_write_number(unsigned int num) {

	int index = 9;
	char nums[11];
	nums[10] = '\0';

	while(index > 0) {
		if(num == 0) {
			break;
		}

		unsigned int mod = num % 16;
		if(mod < 10) {
			nums[index] = mod + '0';
		}
		else {
			nums[index] = mod + 'A' - 10;
		}
		num = num / 16;
		--index;
	}

	console_write_string(nums + index + 1);
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
		console_putchar(string[i]);
	}
}

void console_clear_screen() {

	for(int y = 0; y < CONSOLE_HEIGHT; ++y) {
		for(int x = 0; x < CONSOLE_WIDTH; ++x) {
			console_putchar(' ');
		}
	}
}

void console_putchar(char character) {

	if(character == '\n') {
		system_out.x_pos = 0;
		system_out.y_pos += 1;
	}
	else {
		int index = CONSOLE_WIDTH * system_out.y_pos + system_out.x_pos;
		system_out.buffer[index] = set_vga_entry(character);

		++system_out.x_pos;
	}

	console_check_bounds();
}

static inline void console_check_bounds() {
	if(system_out.x_pos == CONSOLE_WIDTH) {
		system_out.x_pos = 0;
		++system_out.y_pos;
	}

	if(system_out.y_pos == CONSOLE_HEIGHT) {
		system_out.y_pos = 0;
	}
}

static inline uint16_t set_vga_entry(char character) {
	return system_out.color << 8 | character;
}