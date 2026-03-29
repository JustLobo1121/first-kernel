#include "ports.h"

extern void print(char* message, ...);
extern void hex_to_string(unsigned int n, char* str);

unsigned int pci_read_word(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset) {
    unsigned int address; 
    unsigned int lbus  = (unsigned int)bus; 
    unsigned int lslot = (unsigned int)slot; 
    unsigned int lfunc = (unsigned int)func;

    address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((unsigned int)0x80000000));
    
    port_dword_out(0xCF8, address);
    return (unsigned int)((port_dword_in(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

void check_all_pci_buses() {
    print("Scanning Bus PCI...\n");
    for (int device = 0; device < 32; device++) {
        unsigned int vendor_id = pci_read_word(0, device, 0, 0);         
        if (vendor_id != 0xFFFF) {
            unsigned int device_id = pci_read_word(0, device, 0, 2);
            char v_str[11], d_str[11];
            hex_to_string(vendor_id, v_str);
            hex_to_string(device_id, d_str);
            print("device detected! seller: "); 
            print(v_str);
            print(" Device: ");
            print(d_str);
            print("\n");
        }
    }
}