#include "kernel.h"
#include "console.h"
#include "keyboard.h"
#include "pci_ide.h"
#include <stdint.h>
#include <stddef.h>

struct idt_ptr_t idt_ptr ;
struct console stdout;
struct keyboard kbd;
struct idt_descriptor idt_table[256] __attribute__((aligned (8)));
struct ide_driver_data ide_data;

// Must put in own section so that it is loaded at the OS_ENTRY point
__attribute__((section (".os_entry")))
void kernel_start(void) {

	// Init console soon to get debug output
	console_init();
	pci_ide_init();
	keyboard_init();

	// Only enable interrupts once all init is complete.
	// Will probably change this later.
	fill_idt_table();

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

	int prompt_length = strlen(stdout.prompt_string);
	char input_string[MAX_COMMAND_LENGTH + 1];

	while(1) {
		console_print_string(stdout.prompt_string);
		stdout.line_index = prompt_length;
		get_input_command(input_string);
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

		// Print the character we just typed
		if(index >= 0) {
			if(index > 0 || input_char != '\b') {
				console_putchar(input_char);

				// Print the special continuation line if we are past
				// the width of the screen.
				check_input_runoff(input_char);
			}
		}

		// Now record the character in the command string
		// Backspace is special. Shouldn't store that in the command string
		if(input_char == '\b') {
			// Don't let the user backup through the start of the string
			if(index > 0) {
				--index;
				string[index] = '\0';
			}
		}
		else if(input_char == '\t') {
			for(int i = 0; i < CONSOLE_TAB_SIZE; ++i) {
				string[index] = ' ';
				++index;
			}
		}
		// Normal characters
		else {
			string[index] = input_char;
			++index;
		}
	}
}

void check_input_runoff(char input_char) {

	// Update the line index
	if(input_char == '\b') {
		--stdout.line_index;

		if(stdout.line_index < 3) {
			stdout.line_index = CONSOLE_WIDTH - 1;

			for(int i = 0; i < 3; ++i) {
				console_putchar(input_char);
			}
		}
	}
	else if(input_char == '\t') {
		stdout.line_index += 4;
	}
	else {
		++stdout.line_index;
	}

	// Use line index to check if we ran off the edge
	if(stdout.line_index - CONSOLE_WIDTH >= 0) {
		console_print_string(" | ");
		stdout.line_index = 3;
	}
}

// Very simple printing wrapper for reporting kernel events
void kprint(char * string) {

	static char * k_string_starter = " <KERNEL> : ";
	console_print_string(k_string_starter);
	console_print_string(string);
	console_putchar('\n');
}


void fill_idt_table() {

	idt_ptr.limit = sizeof(struct idt_descriptor) * 256 - 1;
	idt_ptr.base = (uint32_t)&idt_table;

	for(int i = 0; i < 256; ++i) {
		if(i == IRQ_KEYBOARD) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&_interrupt_09);
		}
		else if(i == IRQ_PIT) {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&_interrupt_00);
		}
		else {
			idt_table[i] = create_idt_entry((int)&CODE_SELECTOR, (uint32_t)&_interrupt_xx);
		}
	}

	// Load the new table just made
	asm("lidt idt_ptr\n"
		"sti\n");

	kprint("IDT Table setup complete.");
}

struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset) {

	uint32_t first_word = (selector << 16) | (offset & 0xFFFF);
	uint32_t second_word = (offset & 0xFFFF0000) | 0x8F00;

	return (struct idt_descriptor){first_word, second_word};
}

// PIT Timer Handler
void handle_int_00() {

	// Do some cool time related stuff here

	// Send done signal to PIC
	outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
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