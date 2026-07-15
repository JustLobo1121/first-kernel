#include "screen.h"
#include "ports.h"

extern void hex_to_string(unsigned int val, char* dest);
unsigned int e1000_mmio_base = 0;
unsigned char mac_address[6];

struct e1000_tx_desc {
    unsigned long long buffer_addr;
    unsigned short length;
    unsigned char cso;
    unsigned char cmd;
    unsigned char status;
    unsigned char css;
    unsigned short special;
} __attribute__((packed));

#define TX_DESC_COUNT 8
struct e1000_tx_desc tx_ring[TX_DESC_COUNT] __attribute__((aligned(16)));
unsigned char tx_buffers[TX_DESC_COUNT][2048];

unsigned int pci_read_dword(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset) {
    unsigned int address = (unsigned int)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    port_dword_out(0xCF8, address);
    return port_dword_in(0xCFC);
}

void pci_write_dword(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned int data) {
    unsigned int address = (unsigned int)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    port_dword_out(0xCF8, address);
    port_dword_out(0xCFC, data);
}

unsigned int e1000_read_reg(unsigned short offset) {
    volatile unsigned int* reg = (volatile unsigned int*)(e1000_mmio_base + offset);
    return *reg;
}

void e1000_write_reg(unsigned short offset, unsigned int value) {
    volatile unsigned int* reg = (volatile unsigned int*)(e1000_mmio_base + offset);
    *reg = value;
}

void read_mac_address() {
    unsigned int mac_low = e1000_read_reg(0x5400);
    unsigned int mac_high = e1000_read_reg(0x5404);

    mac_address[0] = mac_low & 0xFF;
    mac_address[1] = (mac_low >> 8) & 0xFF;
    mac_address[2] = (mac_low >> 16) & 0xFF;
    mac_address[3] = (mac_low >> 24) & 0xFF;
    mac_address[4] = mac_high & 0xFF;
    mac_address[5] = (mac_high >> 8) & 0xFF;

    print("mac address:\n");
    char hex_str[11];
    for (int i = 0; i < 6; i++) {
        hex_to_string(mac_address[i], hex_str);
        print(hex_str + 8);
        if (i < 5) print(":");
    }
    print("\n");
}

void e1000_init_tx() {
    print("initializing transmission ring (TX)...\n");

    for (int i = 0; i < TX_DESC_COUNT; i++) {
        tx_ring[i].buffer_addr = (unsigned int)tx_buffers[i];
        tx_ring[i].length = 0;
        tx_ring[i].cmd = 0;
        tx_ring[i].status = 0;
    }

    e1000_write_reg(0x3800, (unsigned int)tx_ring);
    e1000_write_reg(0x3804, 0);
    e1000_write_reg(0x3808, TX_DESC_COUNT * 16);
    e1000_write_reg(0x3810, 0);
    e1000_write_reg(0x3818, 0);
    unsigned int tctl = (1 << 1) | (1 << 3) | (0x0F << 4) | (0x3F << 12);
    e1000_write_reg(0x0400, tctl);

    print("TX motor on and descriptors mapped.\n");
}

void init_E1000() {
    print("--- starting network ---\n");
    print("searching card intel pro/1000 (E1000)... \n");

    int found = 0;
    unsigned char target_bus, target_slot;

    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            unsigned int vendor_device = pci_read_dword(bus, slot, 0, 0);
            if (vendor_device == 0x100E8086) {
                target_bus = bus;
                target_slot = slot;
                found = 1;
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        print("Critical error: hardware compatible not found\n");
        return;
    }

    unsigned int bar0 = pci_read_dword(target_bus, target_slot, 0, 0x10);
    unsigned int mmio_address = bar0 & 0xFFFFFFF0;

    unsigned int pci_command = pci_read_dword(target_bus, target_slot, 0, 0x04);
    pci_command |= (1 << 2) | (1 << 1) | (1 << 10); 
    pci_write_dword(target_bus, target_slot, 0, 0x04, pci_command);
 
    e1000_mmio_base = mmio_address;
 
    e1000_write_reg(0x00D8, 0xFFFFFFFF); 
 
    volatile unsigned int clear_interrupt = e1000_read_reg(0x00C0);
 
    print("DMA activated. the card have access to the ram\n");
    
    char hex_address[11];
    hex_to_string(mmio_address, hex_address);
    print("MMIO Base: "); print(hex_address); print("\n");

    read_mac_address();
    e1000_init_tx();
}
