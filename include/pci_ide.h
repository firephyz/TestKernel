#ifndef PCI_IDE_INCLUDED
#define PCI_IDE_INCLUDED

#include <stdint.h>
#include "kinit.h"

// IO Ports for I440FX
#define PMC_CONFADDR	0x0CF8	// PCI Configuration Address Register
#define PMC_CONFDAT		0x0CFC	// PCI Configuration Data Register

struct pci_host_bridge_t {

};

void pci_init();

void load_conf_addr(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_read_8(const uint8_t reg);
uint16_t pci_read_16(const uint8_t reg);
uint32_t pci_read_32(const uint8_t reg);

#endif