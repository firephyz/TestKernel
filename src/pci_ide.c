#include "pci_ide.h"
#include "kinit.h"
#include "console.h"

void check_device(uint8_t bus, uint8_t device, uint8_t function);

void pci_init() {

	// load_conf_addr(0, 0, 0);
	// for(int i = 17; i < 32; ++i) {
	// 	uint32_t result = pci_read_32(i);
	// 	console_print_int(result, PRT_BASE_16);
	// 	console_putchar('\n');
	// }

	for(int bus = 0; bus < 256; ++bus) {
		for(int dev = 0; dev < 32; ++dev) {
			for(int func = 0; func < 8; ++func) {
				check_device(bus, dev, func);
			}
		}
	}
}

void load_conf_addr(uint8_t bus, uint8_t device, uint8_t function) {
	uint32_t addr = 0x80000000
				| (bus << 16)
				| ((device & 0x1F) << 11)
				| ((function & 0x7) << 8);

	outl(PMC_CONFADDR, addr);
}

uint8_t pci_read_8(const uint8_t reg) {

	// Load @reg into the configuration address register
	uint32_t addr = inl(PMC_CONFADDR);
	addr = (addr & ~0xFC) | ((reg & 0x3f) << 2);
	outl(PMC_CONFADDR, addr);

	// Read data from @reg
	return inb(PMC_CONFDAT);
}

uint16_t pci_read_16(const uint8_t reg) {

	// Load @reg into the configuration address register
	uint32_t addr = inl(PMC_CONFADDR);
	addr = (addr & ~0xFC) | ((reg & 0x3f) << 2);
	outl(PMC_CONFADDR, addr);

	// Read data from @reg
	return inw(PMC_CONFDAT);
}

uint32_t pci_read_32(const uint8_t reg) {

	// Load @reg into the configuration address register
	uint32_t addr = inl(PMC_CONFADDR);
	addr = (addr & ~0xFC) | ((reg & 0x3f) << 2);
	outl(PMC_CONFADDR, addr);

	// Read data from @reg
	return inl(PMC_CONFDAT);
}

void check_device(uint8_t bus, uint8_t device, uint8_t function) {

	uint32_t addr = 0x80000000 | (bus << 16) | (device << 11) | (function << 8);
	outl(PMC_CONFADDR, addr);
	// uint32_t result = inl(PMC_CONFDAT);

	// if(!(~result)) return;

	// console_print_string(" DEVICE [");
	// console_print_int(bus, PRT_BASE_16);
	// console_putchar(':');
	// console_print_int(device, PRT_BASE_16);
	// console_putchar(':');
	// console_print_int(function, PRT_BASE_16);
	// console_putchar(']');
	// console_putchar(' ');
	// console_print_int(result, PRT_BASE_16);
	// console_putchar('\n');
}