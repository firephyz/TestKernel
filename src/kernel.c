#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

struct idt_ptr_t idt_ptr ;
struct idt_descriptor idt_table[256] asm("idt_table") __attribute__((aligned (8)));
struct console system_out;
struct keyboard kbd;
int cont = 0;
//char key_codes[8] =  {0x0,  0x0, '1', '2',  '3', '4', '5', '6'};	\
// 					   '7',  '8', '9', '0',  '-', '=', 0x0, '\t',	\
// 					   'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',	\
// 					   'o',  'p', '[', ']',  0x0, 0x0, 'a', 's',	\
// 					   'd',  'f', 'g', 'h',  'j', 'k', 'l', ';',	\
// 					   '\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',	\
// 					   'b',  'n', 'm', ',',  '.', '/', 0x0, '*',	\
// 					   0x0,  ' ', 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,	\
// 					   0x0,  0x0, 0x0, 0x0,  0x0, 0x0, 0x0, '7',	\
// 					   '8',  '9', '-', '4',  '5', '6', '+', '1',	\
// 					   '2',  '3', '0', '.',  0x0, 0x0, 0x0, 0x0};

void kernel_start(void) {

	fill_idt_table();
	console_init();

	console_write_string("Hello OS World!\n");
	console_write_string("\n");
	console_write_string(" > How are you today?\n");
	console_write_string("Testing numbers: ");
	console_write_number((unsigned int)&CODE_SELECTOR);

	while (1) {
		if(cont != 0) {
			asm("int $0x21");
		}
		
		console_write_number(getc());
		console_write_string(" [");
		console_write_number(kbd.start);
		console_write_string(",");
		console_write_number(kbd.end);
		console_write_string("]\n");
	}
}

static inline uint8_t inb(uint16_t port) {
	uint8_t result = 0;
	asm volatile ("inb %1, %0"
		: "=a" (result)
		: "Nd" (port));
	return result;
}

static inline void outb(uint16_t port, uint8_t value) {

}

void console_write_number(unsigned int num) {

	int index = 9;
	char nums[11];
	nums[10] = '\0';

	while(index > 0) {
		if(num == 0) {
			break;
		}

		int mod = num % 16;
		if(mod < 10) {
			nums[index] = mod + '0';
		}
		else {
			nums[index] = mod + 'A' - 10;
		}
		num = num / 16;
		--index;
	}

	console_write_string(nums + index + 1);
}

char getc() {

	while(kbd.start == kbd.end) {

	}

	char result = kbd.buffer[kbd.start];
	++kbd.start;

	return result;
}

void fill_idt_table() {

	idt_ptr.limit = sizeof(struct idt_descriptor) * 256 - 1;
	idt_ptr.base = (uint32_t)&idt_table;

	for(int i = 0; i < 256; ++i) {
		if(i == 0x21) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&handle_int_09);
		}
		else if (i == 0x20) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&handle_int_08);
		}
		else {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&handle_int_xx);
		}
	}

	asm("lidt idt_ptr\n"
		"sti\n");
}

struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset) {

	uint32_t first_word = (selector << 16) | (offset & 0xFFFF);
	uint32_t second_word = (offset & 0xFFFF0000) | 0x8F00;

	return (struct idt_descriptor){first_word, second_word};
}

void console_init() {

	system_out.x_pos = 0;
	system_out.y_pos = 0;
	system_out.color = VGA_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	system_out.buffer = (uint16_t *)0xB8000;

	console_clear_screen();
}

void console_write_string(char * string) {

	for(int i = 0; i < strlen(string); ++i) {
		console_putchar(string[i]);
	}
}

void console_clear_screen() {

	for(int y = 0; y < CONSOLE_HEIGHT; ++y) {
		for(int x = 0; x < CONSOLE_WIDTH; ++x) {
			console_putchar(' ');
		}
	}
}

void console_putchar(char character) {

	if(character == '\n') {
		system_out.x_pos = 0;
		system_out.y_pos += 1;
	}
	else {
		int index = CONSOLE_WIDTH * system_out.y_pos + system_out.x_pos;
		system_out.buffer[index] = set_vga_entry(character);

		++system_out.x_pos;
	}

	console_check_bounds();
}

size_t strlen(char * string) {

	size_t len = 0;
	while(string[len] != '\0') {
		++len;
	}

	return len;
}

static inline void console_check_bounds() {
	if(system_out.x_pos == CONSOLE_WIDTH) {
		system_out.x_pos = 0;
		++system_out.y_pos;
	}

	if(system_out.y_pos == CONSOLE_HEIGHT) {
		system_out.y_pos = 0;
	}
}

static inline uint16_t set_vga_entry(char character) {
	return system_out.color << 8 | character;
}