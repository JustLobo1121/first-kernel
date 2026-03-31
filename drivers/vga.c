unsigned char* vga_mem = (unsigned char*) 0xA0000;

void put_pixel(int x, int y, unsigned char color) {
    if (x>=0 && x<320 && y>=0 && y<200) {
        int offset = (y * 320) + x;
        vga_mem[offset] = color;
    }
}

void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            put_pixel(x+j, y+i, color);
        }
    }
}