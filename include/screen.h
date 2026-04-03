#ifndef SCREEN_H
#define SCREEN_H

#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_WHITE 15
#define GET_VGA_COLOR(bg, fg) ((bg << 4) | fg)

void clear_screen();
void print(char* message, ...);
void print_char(char c);
void backspace_on_screen();
void set_color(unsigned char color);

#endif