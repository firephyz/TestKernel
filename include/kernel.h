#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

extern void * handle_int_08;
extern void * interrupt_09;
extern void * handle_int_xx;
extern uint32_t CODE_SELECTOR;

extern struct keyboard kbd;
extern struct console system_out;
extern char key_codes[256];
extern char key_codes_shift[256];

struct idt_descriptor {
	uint32_t first;
	uint32_t second;
};

struct idt_ptr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));


void kernel_start(void);
struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset);
void fill_idt_table();
size_t strlen(char * string);
char getc();

uint8_t inb(uint16_t port);

void outb(uint16_t port, uint8_t value);

#endif