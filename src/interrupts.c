#include "interrupts.h"
#include "keyboard.h"
#include "console.h"

void idt_table_init() {

	idt_ptr.limit = sizeof(struct idt_descriptor) * 256 - 1;
	idt_ptr.base = (uint32_t)&idt_table;

	for(int i = 0; i < 256; ++i) {
		if(i == IRQ_PIT) {
			idt_table[i] = create_idt_entry(0x08, &handle_int_00);
		}
		else if(i == IRQ_KEYBOARD) {
			idt_table[i] = create_idt_entry(0x08, &handle_int_09);
		}
		else {
			idt_table[i] = create_idt_entry(0x08, &handle_int_xx);
		}
	}

	// Load the new table just made
	asm("lidt idt_ptr\n"
		"sti\n");

	kprint("IDT Table setup complete.");
}

struct idt_descriptor create_idt_entry(uint16_t selector, void (*func)()) {

	uint32_t first_word = (selector << 16) | ((intptr_t)func & 0xFFFF);
	uint32_t second_word = ((intptr_t)func & 0xFFFF0000) | 0x8E00;

	return (struct idt_descriptor){first_word, second_word};
}

// PIT Timer Handler
void handle_int_00(struct interrupt_frame * frame) {

	// Do some cool time related stuff here

	// Send done signal to PIC
	outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
}

// Keyboard interrupt
void handle_int_09(struct interrupt_frame * frame) {

	uint8_t input = inb(KEYBOARD_DATA_PORT);

	// Handle left and right shift
	if(input == 0x2A || input == 0x36) {
		kbd.ctr_mask |= CONTROL_SHIFT;
	}
	else if (input == 0xAA || input == 0xB6) {
		kbd.ctr_mask &= !CONTROL_SHIFT;
	}
	// Only report actual key presses, not break codes
	else if(input < 0x80) {
		// Check if buffer is already full
		if(kbd.end == kbd.start - 1 ||
			(kbd.start == 0 && kbd.end == KBD_BUFFER_SIZE - 1)) {
			return;
		}

		kbd.buffer[kbd.end] = input;

		kbd.end++;
		if(kbd.end == KBD_BUFFER_SIZE) kbd.end = 0;
	}

	// Send done signal to PIC
	outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
}

void handle_int_xx(struct interrupt_frame * frame) {
	//console_print_string("Interrupt: ");
}