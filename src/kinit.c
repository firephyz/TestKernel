#include "kinit.h"
#include "console.h"
#include "keyboard.h"
#include "pci_ide.h"
#include "interrupts.h"
#include <stdint.h>
#include <stddef.h>

struct console stdout;
struct keyboard kbd;
struct idt_ptr_t idt_ptr;
struct idt_descriptor idt_table[256] __attribute__((aligned (8)));

// Must put in own section so that it is loaded at the OS_ENTRY point
void init(void) {

	// Init console soon to get debug output
	pic_init();
	console_init();
	pci_init();
	keyboard_init();

	// Only enable interrupts once all init is complete.
	// Will probably change this later.
	idt_table_init();

	print_cpuid();

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
	}

	// Begin main os prompt loop
	run_prompt();
}

void pic_init() {

	outb(PIC_MASTER_COMMAND_PORT, 0x11);
	outb(PIC_SLAVE_COMMAND_PORT, 0x11);

	outb(PIC_MASTER_DATA_PORT, 0x20);
	outb(PIC_SLAVE_DATA_PORT, 0x28);

	outb(PIC_MASTER_DATA_PORT, 0x04);
	outb(PIC_SLAVE_DATA_PORT, 0x02);

	outb(PIC_MASTER_DATA_PORT, 0x05);
	outb(PIC_SLAVE_DATA_PORT, 0x01);

	outb(PIC_MASTER_DATA_PORT, 0xFC);
	outb(PIC_SLAVE_DATA_PORT, 0xFF);
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

	static char * k_string_starter = " <KERNEL> ";
	console_print_string(k_string_starter);
	console_print_string(string);
	console_putchar('\n');
}

void print_cpuid() {

	int32_t cpuid_buf[4];
	int max_cpuid_query = 1;

	for(int i = 0; i <= max_cpuid_query; ++i) {
		cpuid(i, cpuid_buf);
		console_print_string(" <KERNEL> CPUID_");
		console_print_int(i, PRT_BASE_10);
		console_print_string(" [");
		console_print_int(cpuid_buf[0], PRT_BASE_16);
		console_putchar(':');
		console_print_int(cpuid_buf[1], PRT_BASE_16);
		console_putchar(':');
		console_print_int(cpuid_buf[2], PRT_BASE_16);
		console_putchar(':');
		console_print_int(cpuid_buf[3], PRT_BASE_16);
		console_putchar(']');
		console_putchar('\n');

		if(i == 0) max_cpuid_query = cpuid_buf[0];
	}

	return;
}

size_t strlen(char * string) {

	size_t len = 0;
	while(string[len] != '\0') {
		++len;
	}

	return len;
}