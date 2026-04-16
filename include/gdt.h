#ifndef GDT_H
#define GDT_H

struct gdt_entry_struct {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

struct tss_entry_struct {
    unsigned int prev_tss;
    unsigned int esp0, ss0, esp1, ss1, esp2, ss2;
    unsigned int cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    unsigned int es, cs, ss, ds, fs, gs;
    unsigned int ldt;
    unsigned short trap, iomap_base;
} __attribute__((packed));
typedef struct tss_entry_struct tss_entry_t;

void init_gdt();
void set_kernel_stack(unsigned int esp);

#endif