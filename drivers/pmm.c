#include "pmm.h"

#define PMM_BLOCK 4096
#define PMM_BLOCK_SIZE 4096

unsigned int memory_bitmap[PMM_BLOCK / 32];


void pmm_set_bit(int bit) {
    memory_bitmap[bit/32] |= (1 << (bit % 32));
}

void pmm_clear_bit(int bit) {
    memory_bitmap[bit/32] &= ~(1 << (bit % 32));
}

int pmm_test_bit(int bit) {
    return memory_bitmap[bit / 32] & (1 << (bit % 32));
}

void init_pmm() {
    for(int i=0; i< (PMM_BLOCK / 32); i++) {
        memory_bitmap[i] = 0;
    }
    for (int i=0; i<256; i++) {
        pmm_set_bit(i);
    }
}

void* pmm_alloc_frame() {
    for (int bit=0; bit<PMM_BLOCK; bit++) {
        if (pmm_test_bit(bit) == 0) {
            pmm_set_bit(bit);
            return (void*)(bit*PMM_BLOCK_SIZE);
        }
    }
    return 0;
}

void pmm_free_frame(void* ptr) {
    unsigned int addr = (unsigned int) ptr;
    int bit = addr / PMM_BLOCK_SIZE;
    pmm_clear_bit(bit);
}
