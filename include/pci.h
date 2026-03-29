#ifndef PCI_H
#define PCI_H

unsigned int pci_read_word(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
void check_all_pci_buses();

#endif