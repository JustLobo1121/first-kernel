#ifndef VGA_H
#define VGA_H

void init_vga();
void put_pixel(int x, int y, unsigned int color);
unsigned int get_pixel(int x, int y);
void draw_rectangle(int x, int y, int width, int height, unsigned int color);

#endif