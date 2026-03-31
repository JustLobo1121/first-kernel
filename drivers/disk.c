#include "ports.h"

void read_sector(int lba, char* buffer) {
    port_bytes_out(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    port_bytes_out(0x1F2, 1);
    port_bytes_out(0x1F3, (unsigned char)lba);
    port_bytes_out(0x1F4, (unsigned char)(lba >> 8));
    port_bytes_out(0x1F5, (unsigned char)(lba >> 16));
    port_bytes_out(0x1F7, 0x20);
    unsigned char status;
    do {
        status = port_bytes_in(0x1F7);
    } while (!(status & 0x08));

    unsigned short* buffer_16 = (unsigned short*) buffer;
    for (int i = 0; i < 256; i++) {
        buffer_16[i] = port_word_in(0x1F0);
    }
}

void write_sector(int lba, char* buffer) {
    unsigned char status;
    
    do { status = port_bytes_in(0x1F7); } while (status & 0x80);

    port_bytes_out(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    port_bytes_out(0x1F2, 1);
    port_bytes_out(0x1F3, (unsigned char)lba);
    port_bytes_out(0x1F4, (unsigned char)(lba >> 8));
    port_bytes_out(0x1F5, (unsigned char)(lba >> 16));
    port_bytes_out(0x1F7, 0x30); 
    
    do { status = port_bytes_in(0x1F7); } while (!(status & 0x08));

    unsigned short* buffer_16 = (unsigned short*) buffer;
    for (int i = 0; i < 256; i++) {
        port_word_out(0x1F0, buffer_16[i]);
    }
    
    port_bytes_out(0x1F7, 0xE7); 
    
    do { status = port_bytes_in(0x1F7); } while (status & 0x80);
}