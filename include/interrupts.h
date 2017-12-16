#ifndef INTERRUPTS_INCLUDED
#define INTERRUPTS_INCLUDED

#include "stdint.h"
#include "kinit.h"

// IRQ's as set in boot.s
#define IRQ_PIT			0x20
#define IRQ_KEYBOARD	0x21
#define IRQ_SLAVE		0x22
#define IRQ_SERIAL_2	0x23
#define IRQ_SERIAL_1	0x24
#define IRQ_PARALLEL_2	0x25
#define IRQ_FLOPPY		0x26
#define IRQ_PARALLEL_1	0x27
#define IRQ_MOUSE		0x2C
#define IRQ_DISK_1		0x2E
#define IRQ_DISK_2		0x2F

// Required by gcc to be passed by reference into
// an interrupt handler
struct interrupt_frame {
	uint32_t eflags;
	uint16_t code_segment;
	uint32_t eip;
};

struct idt_descriptor {
	uint32_t first;
	uint32_t second;
};

struct idt_ptr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

extern struct idt_ptr_t idt_ptr;
extern struct idt_descriptor idt_table[256];

// Functions for handling the idt and generic interrupts
void idt_table_init();
struct idt_descriptor create_idt_entry(uint16_t selector, void (*func)());

__attribute__((interrupt)) void handle_int_00(struct interrupt_frame * frame); // PIT
__attribute__((interrupt)) void handle_int_09(struct interrupt_frame * frame); // Keyboard
__attribute__((interrupt)) void handle_int_xx(struct interrupt_frame * frame); // Misc/Unimplemented

#endif