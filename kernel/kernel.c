#include <stdarg.h>
#include "ports.h"
#include "speaker.h"
#include "timer.h"
#include "rtc.h"
#include "disk.h"
#include "pmm.h"
#include "pci.h"
#include "idt.h"
#include "isr.h"
#include "paging.h"
#include "string.h"
#include "mem.h"
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

    char* command = strtok(input, ' ');

    if (strcmp(command, "help") == 0) {
        print("- commands: help, clear, beep, cpuinfo, uptime, time, sleep, lspci, alloc\n");
        print("- commands with args:\n echo [message], color [text], read [number], math [number] [operator] [number]\n");
    } else if (strcmp(command, "echo") == 0) {
        char* arg = strtok(0, ' ');
        while (arg != 0) {
            print(arg);
            print(" ");
            arg = strtok(0, ' ');
        }
        print("\n");
    } else if (strcmp(command, "color") == 0) {
        char* bg_str = strtok(0, ' ');
        char* fg_str = strtok(0, ' ');

        if (bg_str == 0 || fg_str == 0) {
            print("Error: missing agrs\n- use: color [bg:cyan|green|red|blue|white] [font:cyan|green|red|blue|white]\n");
        } else {
            unsigned char bg_color = VGA_BLACK;
            unsigned char fg_color = VGA_WHITE;

            // background colors
            if (strcmp(bg_str, "cyan") == 0) bg_color = VGA_CYAN;
            if (strcmp(bg_str, "green") == 0) bg_color = VGA_GREEN;
            if (strcmp(bg_str, "red") == 0) bg_color = VGA_RED;
            if (strcmp(bg_str, "blue") == 0) bg_color = VGA_BLUE;
            if (strcmp(bg_str, "white") == 0) bg_color = VGA_WHITE;

            // fonts colors
            if (strcmp(fg_str, "cyan") == 0) fg_color = VGA_CYAN;
            if (strcmp(fg_str, "green") == 0) fg_color = VGA_GREEN;
            if (strcmp(fg_str, "red") == 0) fg_color = VGA_RED;
            if (strcmp(fg_str, "blue") == 0) fg_color = VGA_BLUE;
            if (strcmp(fg_str, "white") == 0) fg_color = VGA_WHITE;
            
            current_color = GET_VGA_COLOR(bg_color, fg_color);
            clear_screen();
            print("- color has been update\n");
        }
    } else if (strcmp(command, "read") == 0) {
        char* sector_str = strtok(0, ' ');

        if (sector_str == 0) {
            print("Error: missing args\n - use: read [number_of_sector]\n");
        } else {
            int sector_number = sector_str[0] - '0'; 
            print("reading the sector of the disk...\n");
            char sector_data[512]; 
            read_sector(sector_number, sector_data);
            print("reading is finish. verifying disk firm...\n");
            unsigned char byte_510 = sector_data[510];
            unsigned char byte_511 = sector_data[511];

            if (byte_510 == 0x55 && byte_511 == 0xAA) {
                print("¡SUCCESS! firm 0x55AA found in the sector.\n");
            } else {
                print("Fail. not found the firm of the bloatware.\n");
            }
        }
    } else if (strcmp(command, "math") == 0) {
        char* arg1 = strtok(0, ' ');
        char* arg2 = strtok(0, ' ');
        char* arg3 = strtok(0, ' ');

        if (arg1 == 0 || arg2 == 0 || arg3 == 0) {
            print("Error: missing args\n -use: math [number] [+|-|/|*] [number]\n");
        } else {
            int arg1_num = atoi(arg1);
            int arg2_num = atoi(arg3);
            if (strcmp(arg2, "/") == 0) {
                int result = arg1_num / arg2_num;
                char result_str[10];
                itoa(result, result_str);
                print("the result is: ");print(result_str);print("\n");
            }
        }
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();
    } else if (strcmp(command, "beep") == 0) {
        print("Emitiendo pitido en el hardware...\n");
        beep();
    } else if (strcmp(command, "sleep") == 0) {
        print("killing the cpu\n");
        print("stop getting any interrups\n");
        __asm__ volatile("cli; hlt");
    } else if (strcmp(command, "cpuinfo") == 0) {
        get_cpu_info();
    } else if (strcmp(command, "uptime") == 0) {
        unsigned int seconds = get_uptime_second();
        char str_seconds[32];
        itoa(seconds, str_seconds);
        print("the system has ");
        print(str_seconds);
        print(" seconds on\n");
    } else if (strcmp(command, "time") == 0) {
        print_time();
    } else if (strcmp(command, "alloc") == 0) {
        void* ptr = pmm_alloc_frame();
        if (ptr == 0) {
            print("Error: graphic memory is full\n");
        } else {
            char hex_address[11];
            hex_to_string((unsigned int)ptr, hex_address);
            print("Block of 4KB reserve succesfully in: ");
            print(hex_address);
            print("\n");
        }
    } else if (strcmp(command, "lspci") == 0) {
        check_all_pci_buses();
    } else {
        print("unknow command: ");
        print(command);
        print("\n");
    }
}

void main() {
    clear_screen();
    isr_install();
    // init_paging(); // red screen is not showing
    init_pmm();
    void* heap_start = pmm_alloc_frame();
    init_heap(heap_start, 4096);
    __asm__ volatile("sti");
    print("OS> ");
}