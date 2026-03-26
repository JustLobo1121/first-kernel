#include "../drivers/ports.h"
#include "idt.h"

struct idt_entry idt[IDT_ENTRIES];
struct idt_register idt_reg;

void set_idt_gate(int n, unsigned int handler) {
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
}

void set_idt() {
    idt_reg.base = (unsigned int) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof (struct idt_entry) - 1;
    __asm__ volatile("lidt (%0)" :: "r" (&idt_reg));
}