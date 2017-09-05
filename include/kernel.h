#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

extern void * handle_int_08;
extern void * handle_int_09;
extern void * handle_int_xx;
extern uint32_t CODE_SELECTOR;

extern struct keyboard kbd;
extern struct console system_out;
extern char key_codes[88];

struct idt_descriptor {
	uint32_t first;
	uint32_t second;
};

struct idt_ptr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));



struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset);
void fill_idt_table();
size_t strlen(char * string);
char getc();

static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t value);

#endif