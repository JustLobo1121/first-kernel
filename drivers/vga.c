#include "vga.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

unsigned char* vga_mem = 0; 

void init_vga() {
    unsigned int* phys_base_ptr = (unsigned int*) 0x5028;
    vga_mem = (unsigned char*)(*phys_base_ptr);
}

void put_pixel(int x, int y, unsigned int color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        int offset = (y * SCREEN_WIDTH * 3) + (x * 3);
        
        vga_mem[offset] = color & 0xFF;
        vga_mem[offset + 1] = (color >> 8) & 0xFF;
        vga_mem[offset + 2] = (color >> 16) & 0xFF;
    }
}

unsigned int get_pixel(int x, int y) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        int offset = (y * SCREEN_WIDTH * 3) + (x * 3);
        unsigned int b = vga_mem[offset];
        unsigned int g = vga_mem[offset + 1];
        unsigned int r = vga_mem[offset + 2];
        return (r << 16) | (g << 8) | b;
    }
    return 0;
}

void draw_rectangle(int x, int y, int width, int height, unsigned int color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}