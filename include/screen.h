#ifndef SCREEN_H
#define SCREEN_H

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0x00FFFFFF
#define COLOR_RED   0x00FF0000
#define COLOR_GREEN 0x0000FF00
#define COLOR_BLUE  0x000000FF
#define COLOR_CYAN  0x0000FFFF
#define GET_VGA_COLOR(bg, fg) ((bg << 4) | fg)

void clear_screen();
void print(char* message, ...);
void print_char(char c);
void backspace_on_screen();
void set_color(unsigned int color);
void dump_kernel_log();

#endif