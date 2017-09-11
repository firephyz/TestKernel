#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

// Various IO ports to be used
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

// PIT Timer Ports
// Info from OSDev http://wiki.osdev.org/PIT
// Command Port Bit Info
// [7:6] Channel Select
// 	- 00 Channel 0
//  - 01 Channel 1
// 	- 10 Channel 2
// 	- 11 Read-back Command
// [5:4] Access Mode
//	- 00 Latch Count Value Command
//	- 01 Access mode: lobyte only
// 	- 10 Access mode: hibyte only
//	- 11 Access mode: lobyte/hibyte
// [3:1] Operating Mode
//	- 000 Interrupt on terminal count
// 	- 001 Hardware re-triggerable one-shot
//	- 010 Rate generator
//	- 011 Square wave generator
//	- 100 Software triggered strobe
//	- 101 Hardware triggered strobe
//	- 110 Rate generator (same as 010)
//	- 111 Sqaure wave generator (same as 011)
// [0] BCD/Binary Mode
// 	- 0 16-bit binary
//	- 1 4-digit BCD
#define PIT_CHANNEL_0_PORT	0x40
#define PIT_COMMAND_PORT 	0x43

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

#define MAX_COMMAND_LENGTH 1024

// External hooks to boot.s
extern void * _interrupt_00;
extern void * _interrupt_09;
extern void * _interrupt_xx;
extern uint32_t CODE_SELECTOR;

extern struct keyboard kbd;
extern struct console stdout;

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
void kprint(char * string);
void run_prompt();
void get_input_command(char * string);
void check_input_runoff(char input);

size_t strlen(char * string);

void handle_int_00();

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