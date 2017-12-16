#include "keyboard.h"
#include "kinit.h"
#include "console.h"
#include <stdint.h>

// Filled the table with the special enter key
// so that the kernel prompt works correctly.
// Will probably need to change later so that we can use the keyboard
// like normal in a user program.
char key_codes[256] = {0x0,  0x0, '1', '2',  '3',  '4', '5',  '6',	\
					   '7',  '8', '9', '0',  '-',  '=', '\b', '\t',	\
					   'q',  'w', 'e', 'r',  't',  'y', 'u',  'i',	\
					   'o',  'p', '[', ']',  ENTER_KEYCODE_MAP, 0x0, 'a',  's',	\
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

	kprint("Keyboard init complete.");
}

char getc() {

	// Wait for next key event
	getc_while_loop:
	while(kbd.start == kbd.end) {}

	// Very rarely, qemu will jump out the loop above when
	// kbd.start and kbd.end are actually equal.
	// If that happens, go back to the loop
	if(kbd.start == kbd.end) {
		goto getc_while_loop;
	}

	uint8_t result = kbd.buffer[kbd.start];

	++kbd.start;
	if(kbd.start == KBD_BUFFER_SIZE) {
		kbd.start = 0;
	}

	//console_print_int(result, PRT_BASE_16);

	// Return shift keys if we must
	if(kbd.ctr_mask & CONTROL_SHIFT) {
		return key_codes_shift[result];
	}

	return key_codes[result];
}