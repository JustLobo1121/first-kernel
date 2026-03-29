#include "mem.h"

typedef struct MemoryBlock {
    unsigned int size;
    int free;
    struct MemoryBlock* next;
} MemoryBlock;

MemoryBlock* free_list = 0;

void init_heap(void* start_address, unsigned int total_size) {
    free_list = (MemoryBlock*)start_address;
    free_list -> size = total_size - sizeof(MemoryBlock);
    free_list -> free = 1;
    free_list -> next = 0;
}

void* kmalloc(unsigned int size) {
    MemoryBlock* current = free_list;

    while (current != 0) {
        if (current -> free == 1 && current -> size >= size) {
            current -> free = 0;
            return (void*)(current + 1);
        }
        current = current -> next;
    }
    return 0;
}

void kfree(void* ptr) {
    if (ptr == 0) return;

    MemoryBlock* block = ((MemoryBlock*)ptr - 1);
    block -> free = 1;
}
