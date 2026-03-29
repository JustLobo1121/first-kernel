unsigned int page_directory[1024] __attribute__((aligned(4096)));
unsigned int first_page_table[1024] __attribute__((aligned(4096)));

void init_paging() {
    for(int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 4096) | 3; 
    }

    page_directory[0] = ((unsigned int)first_page_table) | 3;
    for(int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }

    __asm__ volatile(
        "mov %0, %%cr3\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        : 
        : "r" (page_directory)
        : "eax"
    );
}