#ifndef PMM_H
#define PPM_H

void init_pmm();
void* pmm_alloc_frame();
void pmm_free_frame(void* ptr);

#endif