#include "console.h"
#include "kernel.h"
#include <stdint.h>

// Should only be used by internal printing functions
// Sets the character under the current cursor and does nothing else
static inline void console_write_char(char c) {

	int index = CONSOLE_WIDTH * system_out.y_pos + system_out.x_pos;
	system_out.buffer[index] = SET_VGA_ENTRY(c);
	return;
}

void console_init() {

	system_out.x_pos = 0;
	system_out.y_pos = 0;
	system_out.color = VGA_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	system_out.buffer = (uint16_t *)0xB8000;

	console_clear_screen();
	console_move_cursor(0, 0);
}

void console_clear_screen() {

	for(int y = 0; y < CONSOLE_HEIGHT; ++y) {
		for(int x = 0; x < CONSOLE_WIDTH; ++x) {
			console_putchar(' ');
		}
	}
}

void console_print_string(char * string) {

	for(int i = 0; i < strlen(string); ++i) {
		console_putchar(string[i]);
	}
}


void console_print_int(int32_t num, int base) {

	int is_negative = 0;
	if(num < 0) {
		is_negative = 1;
		num = -num;
	}
	int string_length = is_negative;

	// Reserve max space for the string givien the printing base
	// Some add an additional 2 for the "0x" or "0b" beginning
	switch(base) {
		case PRT_BASE_2:
			string_length += 32 + 2;
			break;
		case PRT_BASE_8:
			string_length += 11 + 2;
			break;
		case PRT_BASE_10:
			string_length += 10;
			break;
		case PRT_BASE_16:
			string_length += 8 + 2;
			break;
	}

	int index = string_length - 1;
	char nums[string_length + 1];
	nums[string_length] = '\0';

	// Loop through the number and gather the digits
	while(index >= 0) {
		if(num == 0) {
			break;
		}

		unsigned int mod = num % base;
		if(mod < 10) {
			nums[index] = mod + '0';
		}
		else {
			nums[index] = mod + 'A' - 10;
		}
		num = num / base;
		--index;
	}

	// Append the 0x, 0b, 0o if need be
	if(base != PRT_BASE_10) {
		nums[index - 1] = '0';

		if(base == PRT_BASE_2) {
			nums[index] = 'b';
		}
		else if (base == PRT_BASE_8) {
			nums[index] = 'o';
		}
		else if (base == PRT_BASE_16) {
			nums[index] = 'x';
		}

		index -= 2;
	}

	if(is_negative) {
		nums[index] = '-';
	}

	console_print_string(nums + index + 1 - is_negative);
}

// Set character under cursor and move the cursor as well
void console_putchar(char character) {

	if(character == '\n') {
		system_out.x_pos = 0;
		system_out.y_pos += 1;

		console_check_bounds();
	}
	else if (character == '\b') {

		if(system_out.x_pos == 0) {
			--system_out.y_pos;
			system_out.x_pos = CONSOLE_WIDTH;
		}
		
		--system_out.x_pos;
		console_write_char(' ');
	}
	else if (character == '\t') {
		system_out.x_pos += 4;

		console_check_bounds();
	}
	else if (character == '\0') {
		return;
	}
	else {
		console_write_char(character);
		++system_out.x_pos;

		console_check_bounds();
	}

	console_move_cursor(system_out.x_pos, system_out.y_pos);
}