#include "ports.h"
#include "idt.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x11
#define ICW4_8086 0x01

extern void isr0();
extern void isr14();
extern void isr32();
extern void isr33();
extern void isr46();
extern void clear_screen();
extern void print(char* message, ...);
extern unsigned int current_color;
extern int current_row;
extern int current_col;

void pic_remap() {
    unsigned char a1, a2;

    a1 = port_bytes_in(PIC1_DATA);
    a2 = port_bytes_in(PIC2_DATA);

    port_bytes_out(PIC1_COMMAND, ICW1_INIT);
    port_bytes_out(PIC2_COMMAND, ICW1_INIT);

    port_bytes_out(PIC1_DATA, 0x20);
    port_bytes_out(PIC2_DATA, 0x28);

    port_bytes_out(PIC1_DATA, 0x04);
    port_bytes_out(PIC2_DATA, 0x02);

    port_bytes_out(PIC1_DATA, ICW4_8086);
    port_bytes_out(PIC2_DATA, ICW4_8086);

    port_bytes_out(PIC1_DATA, a1);
    port_bytes_out(PIC2_DATA, a2);
}

void isr0_handler() {
    current_color = 0x1F;
    clear_screen();

    print("\n\n ** KERNEL PANIC - SYSTEM STOPED ** \n\n");
    print("   EXCEPTION 0x00: try of divide of 0.\n");
    print("   the kernel has block the cpu to evade corruption of data.\n");
    print("   please, restar the virtual machine.\n");
}
void isr14_handler() {
    current_color = 0x4F;
    clear_screen();

    print("\n\n ** KERNEL PANIC - SYSTEM STOPED ** \n\n");
    print("   EXCEPTION 0x0E: Page fault, (paging fault).\n");
    print("   The MMU try to read the memory with bad aligment or protected.\n");
    print("   The machine has been blocked per security.\n");
}

void isr_install() {
    pic_remap();
    set_idt_gate(0, (unsigned int)isr0);
    set_idt_gate(14, (unsigned int)isr14);
    set_idt_gate(32, (unsigned int)isr32);
    set_idt_gate(33, (unsigned int)isr33);
    set_idt_gate(46, (unsigned int)isr46);
    set_idt();
}