
unsigned char port_bytes_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al": "=a" (result) : "d" (port));
    return result;
}

void port_bytes_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx": :"a"(data),"d"(port));
}

unsigned short port_word_in(unsigned short port) {
    unsigned short result;

    __asm__ volatile("in %%dx, %%ax": "=a"(result): "d"(port));
    return result;
}

void port_word_out(unsigned short port, unsigned short data) {
    __asm__ volatile("outw %%ax, %%dx":: "a"(data), "d"(port));
}

unsigned int port_dword_in(unsigned int port) {
    unsigned int result;
    __asm__ volatile("inl %%dx, %%eax": "=a"(result): "d"(port));
    return result;
}

void port_dword_out(unsigned short port, unsigned int data) {
    __asm__ volatile("outl %%eax, %%dx":: "a"(data), "d"(port));
}