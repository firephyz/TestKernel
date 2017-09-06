#include "keyboard.h"
#include "kernel.h"
#include "console.h"
#include <stdint.h>

char key_codes[256] = {0x0,  0x0, '1', '2',  '3',  '4', '5',  '6',	\
					   '7',  '8', '9', '0',  '-',  '=', '\b', '\t',	\
					   'q',  'w', 'e', 'r',  't',  'y', 'u',  'i',	\
					   'o',  'p', '[', ']',  '\n', 0x0, 'a',  's',	\
					   'd',  'f', 'g', 'h',  'j',  'k', 'l',  ';',	\
					   '\'', '`', 0x0, '\\', 'z',  'x', 'c',  'v',	\
					   'b',  'n', 'm', ',',  '.',  '/', 0x0,  '*',	\
					   0x0,  ' ', 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  '7',	\
					   '8',  '9', '-', '4',  '5',  '6', '+',  '1',	\
					   '2',  '3', '0', '.',  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0};

char key_codes_shift[256] =  {0x0,  0x0, '!', '@',  '#', '$', '%',  '^',	\
							  '&',  '*', '(', ')',  '_', '+', '\b', '\t',	\
							  'Q',  'W', 'E', 'R',  'T', 'Y', 'U',  'I',	\
							  'O',  'P', '{', '}',  0x0, 0x0, 'A',  'S',	\
							  'D',  'F', 'G', 'H',  'J', 'K', 'L',  ':',	\
							  '\"', '~', 0x0, '|',  'Z', 'X', 'C',  'V',	\
							  'B',  'N', 'M', '<',  '>', '?', 0x0,  '*',	\
							  0x0,  ' ', 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  '7',	\
							  '8',  '9', '-', '4',  '5', '6', '+',  '1',	\
							  '2',  '3', '0', '.',  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0,  0x0};

void keyboard_init() {
	kbd.start = 0;
	kbd.end = 0;
	kbd.ctr_mask = CONTROL_CLEAR;
}

char getc() {

	// Wait for next key event
	while(kbd.start == kbd.end) {}

	uint8_t result = kbd.buffer[kbd.start];

	++kbd.start;
	if(kbd.start == 256) {
		kbd.start = 0;
	}

	// Return shift keys if we must
	if(kbd.ctr_mask & CONTROL_SHIFT) {
		return key_codes_shift[result];
	}

	return key_codes[result];
}

void handle_int_09() {

	// GCC for some reason is not acknowledging that inb
	// and outb replace a register. Thus we must do the compiler's
	// job and store those registers outselves
	asm volatile ("push %eax");

	uint8_t input = inb(KEYBOARD_DATA_PORT);

	// Handle left and right shift
	if(input == 0x2A || input == 0x36) {
		kbd.ctr_mask |= CONTROL_SHIFT;
	}
	else if (input == 0xAA || input == 0xB6) {
		kbd.ctr_mask &= !CONTROL_SHIFT;
	}
	// Only report actual key presses, not break codes
	if(input < 0x80) {
		// Check if buffer is already full
		if(kbd.end == kbd.start - 1 ||
			(kbd.start == 0 && kbd.end == 0xFF)) {
			return;
		}

		kbd.buffer[kbd.end] = input;

		kbd.end++;
		if(kbd.end == 256) kbd.end = 0;
	}

	// Send done signal to PIC
	outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);

	asm volatile ("pop %eax");
}