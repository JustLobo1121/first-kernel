#ifndef MEM_H
#define MEM_H


void init_heap(void* start_address, unsigned int total_size);
void* kmalloc(unsigned int size);
void kfree(void* ptr);

#endif