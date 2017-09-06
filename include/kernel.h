#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define KEYBOARD_DATA_PORT		0x60
#define KEYBOARD_COMMAND_PORT	0x64
#define PIC_MASTER_COMMAND_PORT	0x20
#define PIC_MASTER_DATA_PORT	0x21
#define PIC_SLAVE_COMMAND_PORT	0xA0
#define PIC_SLAVE_DATA_PORT		0xA1
#define VGA_ADDRESS_PORT		0x3D4
#define VGA_DATA_PORT		0x3D5

// VGA Communcation Data
#define VGA_CURSOR_LOC_LOW		0x0F
#define VGA_CURSOR_LOC_HIGH		0x0E

// End of interrupt signal for the PIC
#define PIC_EOI				0x20

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

// External hooks to boot.s
extern void * _interrupt_09;
extern void * _interrupt_xx;
extern uint32_t CODE_SELECTOR;

extern struct keyboard kbd;
extern struct console system_out;

struct idt_descriptor {
	uint32_t first;
	uint32_t second;
};

struct idt_ptr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

// Kernel start point
void kernel_start(void);

size_t strlen(char * string);

// Functions for handling the idt and generic interrupts
struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset);
void fill_idt_table();
void handle_int_xx();

// IO port code
static inline uint8_t inb(uint16_t port) {

	uint8_t result = 0;
	asm volatile ("in %%dx, %%al"
		: "=a" (result)
		: "d" (port));
	return result;
}

static inline void outb(uint16_t port, uint8_t value) {

	asm volatile ("out %%al, %%dx"
		:
		: "d" (port), "a" (value));
	return;
}

#endif