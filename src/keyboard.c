#include "keyboard.h"
#include "console.h"
#include <stdint.h>

char getc() {

	while(kbd.start == kbd.end) {}

	uint8_t result = kbd.buffer[kbd.start];

	++kbd.start;
	if(kbd.start == 0xFF) {
		kbd.start = 0;
	}

	//console_write_number(result);
	//console_putchar(' ');

	return key_codes[result];
}