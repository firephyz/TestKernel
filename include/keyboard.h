#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED

#include <stdint.h>
#include "kernel.h"

// Keyboard control constants
#define CONTROL_CLEAR	0x0
#define CONTROL_SHIFT	0x1

#define KBD_BUFFER_SIZE 	128

// Define the enter key to emit a special unsed char code
// so that the kernel knows when the end of a command is reached.
#define ENTER_KEYCODE_MAP		0xFF

struct keyboard {
	int start;
	int end;
	uint8_t buffer[KBD_BUFFER_SIZE];
	uint8_t ctr_mask;
};

extern char key_codes[256];
extern char key_codes_shift[256];
extern struct keyboard kbd;

void keyboard_init();
char getc();
void handle_int_09();

#endif