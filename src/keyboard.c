#include "keyboard.h"
#include "kernel.h"
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

void handle_int_09() {

	// Buffer is full
	if(kbd.end == kbd.start - 1 ||
		(kbd.start == 0 && kbd.end == 0xFF)) {
		return;
	}

	kbd.buffer[kbd.end] = inb(0x60);

	kbd.end++;
	if(kbd.end == 256) kbd.end = 0;

	outb(0x20, 0x20);
}