#include "gdt.h"

gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;
tss_entry_t tss_entry;

extern void gdt_flush(unsigned int);
extern void tss_flush();

void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void write_tss(int num, unsigned short ss0, unsigned int esp0) {
    unsigned int base = (unsigned int) &tss_entry;
    unsigned int limit = base + sizeof(tss_entry_t);

    gdt_set_gate(num, base, limit, 0xE9, 0x00);
    
    unsigned char* tss_ptr = (unsigned char*)&tss_entry;
    for (int i = 0; i < sizeof(tss_entry_t); i++) {
        tss_ptr[i] = 0;
    }

    tss_entry.ss0  = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs   = 0x0B;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_entry.iomap_base = sizeof(tss_entry_t);
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base  = (unsigned int)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    write_tss(5, 0x10, 0x0);

    gdt_flush((unsigned int)&gdt_ptr);
    tss_flush();
}

void set_kernel_stack(unsigned int esp) {
    tss_entry.esp0 = esp;
}