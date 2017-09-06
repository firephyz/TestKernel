#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED

#include <stdint.h>
#include "kernel.h"

#define CONTROL_CLEAR	0x0
#define CONTROL_SHIFT	0x1

struct keyboard {
	int start;
	int end;
	uint8_t buffer[256];
	uint8_t ctr_mask;
};

extern char key_codes[256];
extern char key_codes_shift[256];
extern struct keyboard kbd;

void keyboard_init();
char getc();
void handle_int_09();

#endif