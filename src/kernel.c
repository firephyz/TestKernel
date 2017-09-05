#include "kernel.h"
#include "console.h"
#include "keyboard.h"
#include <stdint.h>
#include <stddef.h>

struct idt_ptr_t idt_ptr ;
struct console system_out;
struct keyboard kbd;
struct idt_descriptor idt_table[256] asm("idt_table") __attribute__((aligned (8)));
char key_codes[256] =  {0x0,  0x0, '1', '2',  '3', '4', '5', '6',	\
					   '7',  '8', '9', '0',  '-', '=', '\b', '\t',	\
					   'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',	\
					   'o',  'p', '[', ']',  '\n', 0x0, 'a', 's',	\
					   'd',  'f', 'g', 'h',  'j', 'k', 'l', ';',	\
					   '\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',	\
					   'b',  'n', 'm', ',',  '.', '/', 0x0, '*',	\
					   0x0,  ' ', 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, '7',	\
					   '8',  '9', '-', '4',  '5', '6', '+', '1',	\
					   '2',  '3', '0', '.',  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0};

char key_codes_shift[256] =  {0x0,  0x0, '!', '@',  '#', '$', '%', '^',	\
							  '&',  '*', '(', ')',  '_', '+', '\b', '\t',	\
							  'Q',  'W', 'E', 'R',  'T', 'Y', 'U', 'I',	\
							  'O',  'P', '{', '}',  0x0, 0x0, 'A', 'S',	\
							  'D',  'F', 'G', 'H',  'J', 'K', 'L', ':',	\
							  '\"', '~', 0x0, '|',  'Z', 'X', 'C', 'V',	\
							  'B',  'N', 'M', '<',  '>', '?', 0x0, '*',	\
							  0x0,  ' ', 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, '7',	\
							  '8',  '9', '-', '4',  '5', '6', '+', '1',	\
							  '2',  '3', '0', '.',  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
							  0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0};

void kernel_start(void) {

	fill_idt_table();
	console_init();
	keyboard_init();

	while (1) {
		char test[2];
		char c = getc();
		console_putchar(c);
	}
}

uint8_t inb(uint16_t port) {

	uint8_t result = 0;
	asm volatile ("inb %1, %0"
		: "=a" (result)
		: "Nd" (port));
	return result;
}

void outb(uint16_t port, uint8_t value) {

	asm volatile ("outb %1, %0"
		:
		: "Nd" (port), "a" (value));
	return;
}

void fill_idt_table() {

	idt_ptr.limit = sizeof(struct idt_descriptor) * 256 - 1;
	idt_ptr.base = (uint32_t)&idt_table;

	for(int i = 0; i < 256; ++i) {
		if(i == 0x21) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&interrupt_09);
		}
		else if (i == 0x20) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&handle_int_08);
		}
		else {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&handle_int_xx);
		}
	}

	// Load the new table just made
	asm("lidt idt_ptr\n"
		"sti\n");
}

struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset) {

	uint32_t first_word = (selector << 16) | (offset & 0xFFFF);
	uint32_t second_word = (offset & 0xFFFF0000) | 0x8F00;

	return (struct idt_descriptor){first_word, second_word};
}

size_t strlen(char * string) {

	size_t len = 0;
	while(string[len] != '\0') {
		++len;
	}

	return len;
}