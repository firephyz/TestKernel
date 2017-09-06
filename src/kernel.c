#include "kernel.h"
#include "console.h"
#include "keyboard.h"
#include <stdint.h>
#include <stddef.h>

struct idt_ptr_t idt_ptr ;
struct console system_out;
struct keyboard kbd;
struct idt_descriptor idt_table[256] __attribute__((aligned (8)));

// Must put in own section so that it is loaded at the OS_ENTRY point
__attribute__((section (".os_entry")))
void kernel_start(void) {

	fill_idt_table();
	console_init();
	keyboard_init();

	console_print_string("Number test: ");
	console_print_int(0x17, PRT_BASE_10);
	console_putchar('\n');

	while (1) {
		char c = getc();
		console_putchar(c);
	}
}

void fill_idt_table() {

	idt_ptr.limit = sizeof(struct idt_descriptor) * 256 - 1;
	idt_ptr.base = (uint32_t)&idt_table;

	for(int i = 0; i < 256; ++i) {
		if(i == IRQ_KEYBOARD) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&_interrupt_09);
		}
		else {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&_interrupt_xx);
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

void handle_int_xx() {
	return;
}

size_t strlen(char * string) {

	size_t len = 0;
	while(string[len] != '\0') {
		++len;
	}

	return len;
}