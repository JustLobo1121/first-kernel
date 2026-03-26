#include "../drivers/ports.h"
#include "idt.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x11
#define ICW4_8086 0x01

extern void isr32();
extern void isr33();

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

void isr_install() {
    pic_remap();
    set_idt_gate(32, (unsigned int)isr32);
    set_idt_gate(33, (unsigned int)isr33);
    set_idt();
}