#include "screen.h"
#include "ports.h"
#include "string.h"
#include "vga.h"
#include "font.h"
#include <stdarg.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FONT_SCALE 2 
#define MAX_COLS (SCREEN_WIDTH / (8 * FONT_SCALE))
#define MAX_ROWS (SCREEN_HEIGHT / (8 * FONT_SCALE))
#define KLOG_SIZE 4096
char klog_buffer[KLOG_SIZE];
int klog_index = 0;
int klog_wrapped = 0;
int dmesg_active = 0;
extern unsigned char* vga_mem;
unsigned int current_color = 0x00FFFFFF;
int current_col = 0;
int current_row = 0;

void update_cursor(int row, int col) {}

void log_char(char c) {
    if (dmesg_active) return;
    klog_buffer[klog_index++] = c;
    if (klog_index >= KLOG_SIZE) {
        klog_index = 0;
        klog_wrapped = 1;
    }
}

void clear_screen() {
    if (vga_mem == 0) return;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 3; i++) {
        vga_mem[i] = 0;
    }
    current_col = 0;
    current_row = 0;
}

void scroll_screen() {
    if (vga_mem == 0) return;
    int bytes_per_row = SCREEN_WIDTH * 3;
    int row_size_bytes = bytes_per_row * (8 * FONT_SCALE); 
    
    for (int i = 0; i < (SCREEN_HEIGHT * bytes_per_row) - row_size_bytes; i++) {
        vga_mem[i] = vga_mem[i + row_size_bytes];
    }
    for (int i = (SCREEN_HEIGHT * bytes_per_row) - row_size_bytes; i < SCREEN_HEIGHT * bytes_per_row; i++) {
        vga_mem[i] = 0;
    }
    current_row = MAX_ROWS - 1;
    current_col = 0;
}

void draw_char_graphic(char c, int x, int y, unsigned int color) {
    if (c < 32 || c > 127) return;

    const unsigned char* glyph = font8x8[(int)c];
    
    for (int cy = 0; cy < 8; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            if ((glyph[cy] >> (7 - cx)) & 1) {
                draw_rectangle(x + (cx * FONT_SCALE), y + (cy * FONT_SCALE), FONT_SCALE, FONT_SCALE, color);
            } else {
                draw_rectangle(x + (cx * FONT_SCALE), y + (cy * FONT_SCALE), FONT_SCALE, FONT_SCALE, 0);
            }
        }
    }
}

void print_char(char c) {
    log_char(c);

    if (c == '\n') {
        current_row++;
        current_col = 0;
    } else if (c == '\b') {
        backspace_on_screen();
    } else {
        int pixel_x = current_col * (8 * FONT_SCALE);
        int pixel_y = current_row * (8 * FONT_SCALE);
        
        draw_char_graphic(c, pixel_x, pixel_y, current_color);
        
        current_col++;
        if (current_col >= MAX_COLS) {
            current_col = 0;
            current_row++;
        }
    }

    if (current_row >= MAX_ROWS) scroll_screen();
}

void backspace_on_screen() {
    if (current_col > 0) {
        current_col--;
    } else if (current_row > 0) {
        current_row--;
        current_col = MAX_COLS - 1;
    }
    
    int pixel_x = current_col * (8 * FONT_SCALE);
    int pixel_y = current_row * (8 * FONT_SCALE);
    
    draw_rectangle(pixel_x, pixel_y, 8 * FONT_SCALE, 8 * FONT_SCALE, 0);
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

void dump_kernel_log() {
    dmesg_active = 1;
    print("\n--- start of the kernel log (dmesg) ---\n");

    if (klog_wrapped) {
        for (int i=0; i<KLOG_SIZE; i++) {
            print_char(klog_buffer[i]);
        }
    }
    for (int i=0;i<klog_index; i++) {
        print_char(klog_buffer[i]);
    }

    print("\n--- end of the kernel log ---\n");
    dmesg_active = 0;
}

void set_color(unsigned int color) {
    current_color = color;
}