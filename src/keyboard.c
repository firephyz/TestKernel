#include "keyboard.h"
#include "kernel.h"
#include "console.h"
#include <stdint.h>

void keyboard_init() {
	kbd.start = 0;
	kbd.end = 0;
	kbd.ctr_mask = CONTROL_CLEAR;
}

char getc() {

	while(kbd.start == kbd.end) {}

	uint8_t result = kbd.buffer[kbd.start];

	++kbd.start;
	if(kbd.start == 0xFF) {
		kbd.start = 0;
	}

	// console_write_number(result);
	// console_putchar(' ');

	if(kbd.ctr_mask & CONTROL_SHIFT) {
		return key_codes_shift[result];
	}

	return key_codes[result];
}

void handle_int_09() {

	uint8_t input = inb(0x60);

	if(input == 0x2A || input == 0x36) {
		kbd.ctr_mask |= CONTROL_SHIFT;
	}
	else if (input == 0xAA || input == 0xB6) {
		kbd.ctr_mask &= !CONTROL_SHIFT;
	}
	// Is a normal character
	else {
		// Buffer is full
		if(kbd.end == kbd.start - 1 ||
			(kbd.start == 0 && kbd.end == 0xFF)) {
			return;
		}

		kbd.buffer[kbd.end] = input;

		kbd.end++;
		if(kbd.end == 256) kbd.end = 0;
	}

	outb(0x20, 0x20);
}