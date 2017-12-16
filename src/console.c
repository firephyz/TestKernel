#include "console.h"
#include "kinit.h"
#include <stdint.h>

// Should only be used by internal printing functions
// Sets the character under the current cursor and does nothing else
static inline void console_write_char(char c) {

	int index = CONSOLE_WIDTH * stdout.y_pos + stdout.x_pos;
	stdout.buffer[index] = SET_VGA_ENTRY(c);
	return;
}

static inline void set_console_state(int value) {
	stdout.state = value;
}

static inline void console_record_character(char character) {

	if(character == '\t') {
		for(int i = 0; i < CONSOLE_TAB_SIZE; ++i) {
			stdout.char_buffer[stdout.char_index] = ' ';
			++stdout.char_index;
		}
	}
	else {
		if(character != '\b' && character != '\0') {
			stdout.char_buffer[stdout.char_index] = character;
			++stdout.char_index;
			if(stdout.char_index == CONSOLE_SIZE) {
				stdout.char_index = 0;
			}
		}
	}
}

void console_init() {

	stdout.x_pos = 0;
	stdout.y_pos = 0;
	stdout.color = VGA_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	stdout.buffer = (uint16_t *)0xB8000;
	stdout.char_index = 0;

	stdout.prompt_string = " > ";
	stdout.line_index = strlen(stdout.prompt_string);

	set_console_state(stdout.state | CONSOLE_IS_RESETING);
	console_clear_screen();
	// Also clear character buffer
	for(int i = 0; i < CONSOLE_SIZE; ++i) {
		stdout.char_buffer[i] = '\0';
	}

	stdout.x_pos = 0;
	stdout.y_pos = 0;
	console_move_cursor(0, 0);
	set_console_state(stdout.state & ~CONSOLE_IS_RESETING);

	kprint("Text console initialized.");
}

void console_clear_screen() {

	stdout.x_pos = 0;
	stdout.y_pos = 0;
	set_console_state(stdout.state | CONSOLE_FREEZE_CURSOR);
	// Clear on screen text buffer
	for(int y = 0; y < CONSOLE_HEIGHT; ++y) {
		for(int x = 0; x < CONSOLE_WIDTH; ++x) {
			console_putchar(' ');
		}
	}
	set_console_state(stdout.state & ~CONSOLE_FREEZE_CURSOR);
	stdout.x_pos = 0;
	stdout.y_pos = 0;
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

	// Only record the character if we are not clearing it.
	if(!(stdout.state & CONSOLE_IS_SCROLLING)) {
		console_record_character(character);
	}

	if(character == '\n') {
		stdout.x_pos = 0;
		stdout.y_pos += 1;

		console_check_bounds();
	}
	else if (character == '\b') {

		if(stdout.x_pos == 0) {
			--stdout.y_pos;
			stdout.x_pos = CONSOLE_WIDTH;
		}
		
		--stdout.x_pos;
		--stdout.char_index;
		console_write_char(' ');
	}
	else if (character == '\t') {
		stdout.x_pos += CONSOLE_TAB_SIZE;

		console_check_bounds();
	}
	else if (character == '\0') {
		return;
	}
	else {
		console_write_char(character);
		++stdout.x_pos;

		console_check_bounds();
	}

	// Only update the cursor position if it isn't frozen
	if(!(stdout.state & CONSOLE_FREEZE_CURSOR)) {
		console_move_cursor(stdout.x_pos, stdout.y_pos);
	}
}

// Handle console boundary stuff
void console_check_bounds() {

	if(stdout.x_pos == CONSOLE_WIDTH) {
		stdout.x_pos = 0;
		++stdout.y_pos;
	}

	// Only process the y hitting the bottom if we aren't
	// reseting the console and aren't already scrolling. 
	// We don't need to scroll a blank screen.
	if((stdout.y_pos >= CONSOLE_HEIGHT)
		&& !(stdout.state & CONSOLE_IS_SCROLLING)
		&& !(stdout.state & CONSOLE_IS_RESETING)) {

		if(!(stdout.state & CONSOLE_IS_SCROLLING)) {
			set_console_state(stdout.state | CONSOLE_IS_SCROLLING);
		}

		// NOT CURRENT CONFIGURATION
		// USE SCROLLING INSTEAD
		// // Wrap bottom up screen back to the top
		// stdout.y_pos = 0;

		// Remove the top line of console characters
		// Find the new line
		int index = 0;
		int newline_index = -1;
		while((stdout.char_buffer[index] != '\n')
			&& (index < CONSOLE_WIDTH)) {

			++index;

			// No new line encountered. Scroll everything up
			if(index == CONSOLE_WIDTH) {
				index--;
				break;
			}
		}
		newline_index = index + 1;

		// Copy all remaining characters back to the
		// beginning of the buffer
		// Assuming the end of the characters will end with a null
		// terminator. Let's hope that's true
		++index;
		while(index < stdout.char_index) {

			stdout.char_buffer[index - newline_index] = stdout.char_buffer[index];
			++index;
		}
		stdout.char_index -= newline_index;

		// Redraw the updated character buffer
		console_clear_screen();
		set_console_state(stdout.state | CONSOLE_FREEZE_CURSOR);
		for(index = 0; index < stdout.char_index; ++index) {
			console_putchar(stdout.char_buffer[index]);
		}
		set_console_state(stdout.state & ~CONSOLE_FREEZE_CURSOR);

		// Reset console variables to the new line at the bottom we just
		// made room for and stop scrolling
		stdout.x_pos = 0;
		stdout.y_pos = CONSOLE_HEIGHT - 1;
		set_console_state(stdout.state & ~CONSOLE_IS_SCROLLING);
	}
}