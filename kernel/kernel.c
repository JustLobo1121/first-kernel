#include "screen.h"
#include "isr.h"
#include "pmm.h"
#include "mem.h"
#include "timer.h"
#include "task.h"
#include "vga.h"
#include "gdt.h"
extern void init_mouse();

void task_a() {
    while (1) {
        print("A");
        for (volatile int i=0; i<5000000;i++);
    }
}
void task_b() {
    while (1) {
        print("B");
        for (volatile int i=0; i<5000000;i++);
    }
}

void main() {
    init_vga();
    clear_screen();
    init_gdt();
    isr_install();
    init_pmm();
    void* heap_start = pmm_alloc_frame();
    init_heap(heap_start, 4096);
    init_multitasking();
    init_timer(100);
    init_mouse();
    __asm__ volatile("sti");
    print("OS> ");
}