#ifndef _KINIT_INCLUDED
#define _KINIT_INCLUDED

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

#define MAX_COMMAND_LENGTH 1024

extern struct keyboard kbd;
extern struct console stdout;

// Kernel start point
__attribute__((section (".init_entry"))) void init(void);
void pic_init();
void kprint(char * string);
void run_prompt();
void get_input_command(char * string);
void check_input_runoff(char input);
void print_cpuid();

size_t strlen(char * string);

// IO port code
static inline uint8_t inb(uint16_t port) {

	uint8_t result = 0;
	asm volatile ("in %1, %0"
		: "=a" (result)
		: "dN" (port));
	return result;
}

static inline void outb(uint16_t port, uint8_t value) {

	asm volatile ("out %1, %0"
		:
		: "dN" (port), "a" (value));
	return;
}

static inline uint16_t inw(uint16_t port) {

	uint16_t result = 0;
	asm volatile ("inw %1, %0"
		: "=a" (result)
		: "dN" (port));
	return result;
}

static inline void outw(uint16_t port, uint16_t value) {

	asm volatile ("outw %1, %0"
		:
		: "dN" (port), "a" (value));
	return;
}


static inline uint32_t inl(uint16_t port) {

	uint32_t result = 0;
	asm volatile("inl %1, %0"
		: "=a" (result)
		: "dN" (port));
	return result;
}

static inline void outl(uint16_t port, uint32_t value) {

	asm volatile ("out %1, %0"
		:
		: "dN" (port), "a" (value));
	return;
}

static inline void cpuid(uint32_t eax_value, uint32_t * results) {

	asm volatile ("mov %0, %%eax\n\
				   cpuid\n\
				   mov %%eax, 0(%1)\n\
				   mov %%ebx, 4(%1)\n\
				   mov %%ecx, 8(%1)\n\
				   mov %%edx, 12(%1)\n"
		: 
		: "g" (eax_value), "p" (results)
		: "eax", "ebx", "ecx", "edx");
}

#endif