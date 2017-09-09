#include "kernel.h"
#include "console.h"
#include "keyboard.h"
#include <stdint.h>
#include <stddef.h>

struct idt_ptr_t idt_ptr ;
struct console stdout;
struct keyboard kbd;
struct idt_descriptor idt_table[256] __attribute__((aligned (8)));

// Must put in own section so that it is loaded at the OS_ENTRY point
__attribute__((section (".os_entry")))
void kernel_start(void) {

	fill_idt_table();
	console_init();
	keyboard_init();

	// Print the welcome message.
	char * welcome = "\n\
 ***************************\n\
 *        Welcome to       *\n\
 *        <OS_NAME>        *\n\
 *                         *\n\
 * Use at your own risk... *\n\
 ***************************\n\n";
 	int index = 0;
 	while(welcome[index] != '\0') {
 		console_putchar(welcome[index]);
 		++index;
 		// Add small delay. It looks cool :)
 		for(int i = 0; i < 5000000; ++i) {}
 	}

 	// Begin main os prompt loop
	run_prompt();
}

void run_prompt() {

	char * prompt_string = " > ";
	char input_string[MAX_COMMAND_LENGTH + 1];

	while(1) {
		console_print_string(prompt_string);
		get_input_command(input_string);

		//console_print_string("\n => ");
		//console_print_string(input_string);
		console_putchar('\n');
	}
}

void get_input_command(char * string) {

	int index = 0;
	char input_char = '\0';

	while(index < MAX_COMMAND_LENGTH) {

		input_char = getc();

		// Enter key is special. Signals end of command
		if((unsigned char)input_char == ENTER_KEYCODE_MAP) {
			string[index] = '\0';
			return;
		}
		// Backspace is also special. Shouldn't store that in the command string
		else if(input_char == '\b') {
			// Don't let user back up into the prompt. That's bad.
			if(index != 0) {
				--index;
				string[index] = '\0';
				console_putchar(input_char);
			}
		}
		// Normal characters
		else {
			console_putchar(input_char);
			string[index] = input_char;
			++index;
		}
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