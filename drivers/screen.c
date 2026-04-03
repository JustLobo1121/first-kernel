#include "screen.h"
#include "ports.h"
#include "string.h"
#include "vga.h"
#include "font.h"
#include <stdarg.h>

#define VGA_GRAPHIC_MEM 0xA0000
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define MAX_COLS 40
#define MAX_ROWS 25

unsigned char current_color = 15;
int current_col = 0;
int current_row = 0;

void update_cursor(int row, int col) {}

void clear_screen() {
    unsigned char* vga = (unsigned char*) VGA_GRAPHIC_MEM;
    for (int i=0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++) {
        vga[i] = 0;
    }
    current_col = 0;
    current_row = 0;
}

void scroll_screen() {
    unsigned char* vga = (unsigned char*) VGA_GRAPHIC_MEM;
    for (int i=0; i < SCREEN_WIDTH* (SCREEN_HEIGHT - 8); i++) {
        vga[i] = vga[i+2560];
    }
    for (int i = SCREEN_WIDTH * (SCREEN_HEIGHT - 8); i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga[i] = 0;
    }
    current_row = MAX_ROWS - 1; 
    current_col = 0;
}

void draw_char_graphic(char c, int x, int y, unsigned char color) {
    if (c < 32 || c > 127) return;

    const unsigned char* glyph = font8x8[(int)c];
    
    for (int cy = 0; cy < 8; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            if ((glyph[cy] >> (7 - cx)) & 1) {
                put_pixel(x + cx, y + cy, color);
            } else {
                put_pixel(x + cx, y + cy, 0);
            }
        }
    }
}

void print_char(char c) {
    if (c == '\n') {
        current_row++;
        current_col = 0;
    } else if (c == '\b') {
        backspace_on_screen();
    } else {
        int pixel_x = current_col * 8;
        int pixel_y = current_row * 8;
        
        draw_char_graphic(c, pixel_x, pixel_y, current_color);
        
        current_col++;
        if (current_col >= MAX_COLS) {
            current_col = 0;
            current_row++;
        }
    }

    if (current_row >= MAX_ROWS) {
        scroll_screen();
    }
}

void backspace_on_screen() {
    if (current_col > 0) {
        current_col--;
    } else if (current_row > 0) {
        current_row--;
        current_col = MAX_COLS - 1;
    }
    
    int pixel_x = current_col * 8;
    int pixel_y = current_row * 8;
    
    draw_rectangle(pixel_x, pixel_y, 8, 8, 0);
}

void print(char* message, ...) {
    va_list args;
    va_start(args, message);
    int i=0;
    while (message[i] != 0) {
        if (message[i] == '%' && message[i+1] == 'd') {
            int num = va_arg(args, int);
            char str_num[32];
            itoa(num,str_num);
            int j=0;
            while (str_num[j] != 0) {
                print_char(str_num[j]);
                j++;
            }
            i += 2;
            continue;
        }
        print_char(message[i]);
        i++;
    }
    va_end(args);
}

void set_color(unsigned char color) {
    current_color = color;
}