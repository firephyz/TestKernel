#include "keyboard.h"
#include <stdint.h>

char getc() {

	while(kbd.start == kbd.end) {}

	uint8_t result = kbd.buffer[kbd.start];

	++kbd.start;
	if(kbd.start == 0xFF) {
		kbd.start = 0;
	}

	return key_codes[result];
}