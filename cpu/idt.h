#ifndef CPU_IDT_H
#define CPU_IDT_H

struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_register {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

#define IDT_ENTRIES 256

void set_idt_gate(int n, unsigned int handler);
void set_idt();

#endif