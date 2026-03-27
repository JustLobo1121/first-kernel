#include <stdarg.h>
#include "../drivers/ports.h"
#include "../drivers/speaker.h"
#include "../drivers/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../libc/string.h"
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_WHITE 15
#define GET_VGA_COLOR(bg, fg) ((bg << 4) | fg)
unsigned char current_color = 0x0f;
int current_row=0;
int current_col=0;

void update_cursor(int row, int col);
void print_char(char c);
void clear_screen();
void scroll_screen();
void reverse(char str[], int length);
void itoa(int n, char str[]);
void print(char* message, ...);
void get_cpu_info();

void get_cpu_info() {
    int ebx, ecx, edx;
    __asm__ volatile("cpuid": "=b"(ebx),"=c"(ecx),"=d"(edx):"a"(0));
    char vendor[13];
    *((int*)&vendor[0]) = ebx;
    *((int*)&vendor[4]) = edx;
    *((int*)&vendor[8]) = ecx;
    vendor[12] = '\0';
    print("procesor found: ");
    print(vendor);
    print("\n");
}

void clear_screen() {
    char* screen = (char*) VIDEO_ADDRESS;
    for (int i = 0; i < MAX_COLS * MAX_ROWS; i++) {
        screen[i * 2] = ' ';
        screen[i * 2 + 1] = current_color;
    }
    current_col = 0;
    current_row = 0;
    update_cursor(0,0);
}

void scroll_screen() {
    char* screen = (char*) VIDEO_ADDRESS;
    
    for (int i = 1; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            int origin = (i * MAX_COLS + j) * 2;
            
            int destiny = ((i - 1) * MAX_COLS + j) * 2;

            screen[destiny] = screen[origin];
            screen[destiny + 1] = screen[origin + 1];
        }
    }
    for (int j = 0; j < MAX_COLS; j++) {
        int offset = ((MAX_ROWS - 1) * MAX_COLS + j) * 2;
        screen[offset] = ' ';
        screen[offset + 1] = current_col;
    }
    current_row = MAX_ROWS - 1; 
    current_col = 0;
}

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void itoa(int n, char str[]) {
    int i = 0;
    int is_negative = 0;
    if (n < 0) {
        is_negative = 1;
        n = -n;
    }
    do {
        str[i++] = (n % 10) + '0';
    } while ((n/=10) > 0);

    if (is_negative) str[i++]= '-';
    str[i] = '\0';
    reverse(str, i);
}

void update_cursor(int row, int col) {
    unsigned short offset = (row*80) + col;
    port_bytes_out(0x3D4, 14);
    port_bytes_out(0x3D5, (unsigned char)(offset>>8));
    port_bytes_out(0x3D4, 15);
    port_bytes_out(0x3D5, (unsigned char)(offset & 0xff));
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

void print_char(char c) {
    char* screen = (char*) VIDEO_ADDRESS;

    if (c == '\n') {
        current_row++;
        current_col = 0;
    } else if (c == '\b') {
        if (current_col > 0) {
            current_col--;
        } else if (current_row > 0) {
            current_row--;
            current_col = MAX_COLS - 1;
        }
        int offset = (current_row * MAX_COLS + current_col) * 2;
        screen[offset] = ' ';
        screen[offset + 1] = current_color;
    }else {
        int offset = (current_row * MAX_COLS + current_col) * 2;
        screen[offset] = c;
        screen[offset + 1] = current_color;
        current_col++;

        if (current_col >= MAX_COLS) {
            current_col = 0;
            current_row++;
        }
    }

    if (current_row >= MAX_ROWS) {
        scroll_screen();
    }
    update_cursor(current_row, current_col);
}

void execute_command(char* input) {
    if (input[0] == '\0') return;

    if (strcmp(input, "help") == 0) {
        print("commands:\n");
        print("- help  : show this menu\n");
        print("- clear : clear the screen\n");
        print("- beep : make an soud within the hardware\n");
        print("- sleep : kill the interrupts and get into deep sleep\n");
        print("- cpuinfo : return the tipe of the procesor\n");
        print("- uptime : show the ticks per second\n");
        print("- echo [message] :  make the os to print the message\n");
        print("- color [option] : change the color to [option]\n- [option]: green, cyan, red, blue, white\n");

    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
    } else if (strcmp(input, "beep") == 0) {
        print("Emitiendo pitido en el hardware...\n");
        beep();
    } else if (strcmp(input, "sleep") == 0) {
        print("killing the cpu\n");
        print("stop getting any interrups\n");
        __asm__ volatile("cli; hlt");
    } else if (strcmp(input, "cpuinfo") == 0) {
        get_cpu_info();
    } else if (strcmp(input, "uptime") == 0) {
        unsigned int seconds = get_uptime_second();
        char str_seconds[32];
        itoa(seconds, str_seconds);
        print("the system has ");
        print(str_seconds);
        print(" seconds on\n");
    } else if (strncmp(input, "echo ", 5) == 0) {
        print(&input[5]);
        print("\n");
    } else if (strncmp(input, "color ", 6) == 0) {
        char* color_name = &input[6];
        if (strcmp(color_name, "cyan") == 0) {
            current_color = GET_VGA_COLOR(VGA_BLACK, VGA_CYAN);
            clear_screen();
            print("switch to cyan\n");
        } else if (strcmp(color_name, "green") == 0) {
            current_color = GET_VGA_COLOR(VGA_BLACK, VGA_GREEN);
            clear_screen();
            print("switch to green\n");
        } else if (strcmp(color_name, "red") == 0) {
            current_color = GET_VGA_COLOR(VGA_BLACK, VGA_RED);
            clear_screen();
            print("switch to red\n");
        } else if (strcmp(color_name, "blue") == 0) {
            current_color = GET_VGA_COLOR(VGA_BLACK, VGA_BLUE);
            clear_screen();
            print("switch to blue\n");
        } else if (strcmp(color_name, "white") == 0) {
            current_color = GET_VGA_COLOR(VGA_BLACK, VGA_WHITE);
            clear_screen();
            print("switch to white\n");
        } else {
            print("color not recognize.\n");
        }
    } else {
        print("unknow command: ");
        print(input);
        print("\n");
    }
}

void main() {
    clear_screen();
    isr_install();
    __asm__ volatile("sti");
    print("OS> ");
}