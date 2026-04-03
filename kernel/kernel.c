#include "screen.h"
#include "isr.h"
#include "pmm.h"
#include "mem.h"

void main() {
    clear_screen();
    isr_install();
    // init_paging(); // red screen is not showing
    init_pmm();
    void* heap_start = pmm_alloc_frame();
    init_heap(heap_start, 4096);
    __asm__ volatile("sti");
    print("OS> ");
}