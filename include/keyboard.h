#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED

#include <stdint.h>
#include "kernel.h"

struct keyboard {
	int start;
	int end;
	uint8_t buffer[256];
};

char getc();

#endif