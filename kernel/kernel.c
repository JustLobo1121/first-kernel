#include "screen.h"
#include "isr.h"
#include "pmm.h"
#include "mem.h"
#include "timer.h"
#include "task.h"

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
    clear_screen();
    isr_install();
    init_pmm();
    void* heap_start = pmm_alloc_frame();
    init_heap(heap_start, 4096);
    init_multitasking();
    create_task(task_a);
    create_task(task_b);
    init_timer(100);
    __asm__ volatile("sti");
    while (1) {
        print("C");
        for (volatile int i=0; i<5000000;i++);
    }
}