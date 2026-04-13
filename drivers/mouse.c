#include "ports.h"
#include "screen.h"
#include "vga.h"

char mouse_bytes[3];
int mouse_cycle = 0;
int mouse_x = 400;
int mouse_y = 300;
int prev_mouse_x = 400;
int prev_mouse_y = 300;

int mouse_pointer[16][16] = {
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,1,1,1,1,0,0,0,0},
    {1,2,2,1,1,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,1,2,2,1,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,1,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0}
};

unsigned int mouse_back_buffer[256]; 

void save_background(int x, int y) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            mouse_back_buffer[i * 16 + j] = get_pixel(x + j, y + i);
        }
    }
}

void restore_background(int x, int y) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            put_pixel(x + j, y + i, mouse_back_buffer[i * 16 + j]);
        }
    }
}

void draw_mouse_pointer(int x, int y, unsigned int fill_color) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (mouse_pointer[i][j] == 1) {
                put_pixel(x + j, y + i, 0x00000000);
            } else if (mouse_pointer[i][j] == 2) {
                put_pixel(x + j, y + i, fill_color);
            }
        }
    }
}

void mouse_wait(unsigned char a_type) {
    int timeout = 100000;
    if (a_type == 0) {
        while (timeout--) { if ((port_bytes_in(0x64) & 1) == 1) return; }
        return;
    } else {
        while (timeout--) { if ((port_bytes_in(0x64) & 2) == 0) return; }
        return;
    }
}
void mouse_write(unsigned char a_write) {
    mouse_wait(1); port_bytes_out(0x64, 0xD4);
    mouse_wait(1); port_bytes_out(0x60, a_write);
}
void init_mouse() {
    unsigned char status;
    mouse_wait(1); port_bytes_out(0x64, 0xA8);
    mouse_wait(1); port_bytes_out(0x64, 0x20); mouse_wait(0);
    status = port_bytes_in(0x60) | 2;
    mouse_wait(1); port_bytes_out(0x64, 0x60); mouse_wait(1);
    port_bytes_out(0x60, status);
    mouse_write(0xF6); mouse_wait(0); port_bytes_in(0x60);
    mouse_write(0xF4); mouse_wait(0); port_bytes_in(0x60);
}


void mouse_handler() {
    unsigned char status = port_bytes_in(0x64);
    if (!(status & 0x20)) goto end_interrupt;
    mouse_bytes[mouse_cycle++] = port_bytes_in(0x60);

    if (mouse_cycle == 3) {
        mouse_cycle = 0;
        if ((mouse_bytes[0] & 0x08) == 0) goto end_interrupt;

        int delta_x = mouse_bytes[1];
        int delta_y = mouse_bytes[2];

        if (mouse_bytes[0] & 0x10) delta_x |= 0xFFFFFF00;
        if (mouse_bytes[0] & 0x20) delta_y |= 0xFFFFFF00;
        restore_background(prev_mouse_x, prev_mouse_y);

        mouse_x += delta_x;
        mouse_y -= delta_y;

        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x > 784) mouse_x = 784; 
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y > 584) mouse_y = 584;
        save_background(mouse_x, mouse_y);

        unsigned int fill_color = 0x00FFFFFF; 
        if (mouse_bytes[0] & 0x01) {
            fill_color = 0x0000FF00; 
        }
        draw_mouse_pointer(mouse_x, mouse_y, fill_color);

        prev_mouse_x = mouse_x;
        prev_mouse_y = mouse_y;
    }
    
end_interrupt:
    port_bytes_out(0xA0, 0x20); 
    port_bytes_out(0x20, 0x20); 
}