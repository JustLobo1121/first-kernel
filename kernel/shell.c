#include "shell.h"
#include "screen.h"
#include "string.h"
#include "speaker.h"
#include "timer.h"
#include "rtc.h"
#include "disk.h"
#include "pmm.h"
#include "pci.h"
#include "vga.h"
#include "cpu_utils.h"

extern void read_fs_info();
extern void list_files();
extern void create_dummy_file();
extern void make_directory(char* dirname);
extern void cat_file(char* filename);
extern void change_directory(char* dirname);
extern void remove_file(char* filename);
extern void execute_program(char* filename);
void write_file(char* filename, char* content);

void cmd_help();
void cmd_echo();
void cmd_color();
void cmd_read();
void cmd_math();
void cmd_write();
void cmd_draw();
void cmd_fsinfo();
void cmd_ls();
void cmd_mkdummy();
void cmd_mkdir();
void cmd_cd();
void cmd_cat();
void cmd_crash();
void cmd_clear();
void cmd_beep();
void cmd_sleep();
void cmd_cpuinfo();
void cmd_uptime();
void cmd_time();
void cmd_alloc();
void cmd_lspci();
void cmd_rm();
void cmd_run();
void cmd_mkapp();

typedef void (*command_func)();

typedef struct {
    char* name;
    command_func handler;
    char* description;
} command_t;

command_t commands[] = {
{"help", cmd_help, "shows all the commands"},
{"echo", cmd_echo, "print a message"},
{"color", cmd_color, " changes the color the background and the fonts"},
{"read", cmd_read, "read one sector of the disk"},
{"math", cmd_math, "can make basic arithmetic functions"},
{"write", cmd_write, "can write something in one sector"},
{"draw", cmd_draw, "can draw a rectangle"},
{"fsinfo", cmd_fsinfo, "return the disk information"},
{"ls", cmd_ls, "return al files available"},
{"mkdummy", cmd_mkdummy, "make a dummy file for test"},
{"mkdir", cmd_mkdir, "make a new directory"},
{"cd", cmd_cd, "change the directory actual"},
{"cat", cmd_cat, "read a file within the system"},
{"crash", cmd_crash, "force the page fault"},
{"clear", cmd_clear, "clear the screen"},
{"beep", cmd_beep, "make a beep sound"},
{"sleep", cmd_sleep, "kill the interrupts of the system"},
{"cpuinfo", cmd_cpuinfo, "return the cpu information"},
{"uptime", cmd_uptime, "return time since system bootup"},
{"time", cmd_time, "return the local time of hardwar"},
{"alloc", cmd_alloc, "make an reserve of memory"},
{"lspci", cmd_lspci, "return the pci buses available"},
{"rm", cmd_rm, "delete a file or directory from the disk"},
{"run", cmd_run, "execute a program"},
{"mkapp", cmd_mkapp, "make a executeable progrma"}
};
#define NUM_SHELL_COMMAND (sizeof(commands) / sizeof(command_t))

void execute_command(char* input) {
    if (input[0] == '\0') return;

    char* command = strtok(input, ' ');

    for (int i=0;i<NUM_SHELL_COMMAND;i++) {
        if (strcmp(command, commands[i].name) == 0) {
            commands[i].handler();
            return;
        }
    }
    print("unknow command: ");
    print(command);
    print("\n");
}

void cmd_help() {
    print("--- commands availables ---\n");
    for (int i=0; i<NUM_SHELL_COMMAND;i++) {
        print("- ");
        print(commands[i].name);
        print(": ");
        print(commands[i].description);
        print("\n");
    }
    print("-----------------\n");
}
void cmd_echo() {
    char* arg = strtok(0, ' ');
    while (arg != 0) {
        print(arg);
        print(" ");
        arg = strtok(0, ' ');
    }
    print("\n");
}
void cmd_color() {
    char* fg_str = strtok(0, ' ');
    if (fg_str == 0) {
        print("Error: missing agrs\n- use: color [cyan|green|red|blue|white]\n");
    } else {
        unsigned int fg_color = COLOR_WHITE;
        
        if (strcmp(fg_str, "cyan") == 0) fg_color = COLOR_CYAN;
        if (strcmp(fg_str, "green") == 0) fg_color = COLOR_GREEN;
        if (strcmp(fg_str, "red") == 0) fg_color = COLOR_RED;
        if (strcmp(fg_str, "blue") == 0) fg_color = COLOR_BLUE;
        if (strcmp(fg_str, "white") == 0) fg_color = COLOR_WHITE;
        
        set_color(fg_color);
        clear_screen();
        print("- color has been update\n");
    }
}
void cmd_read() {
    char* sector_str = strtok(0, ' ');
    if (sector_str == 0) {
        print("Error: missing args\n - use: read [number_of_sector]\n");
    } else {
        int sector_number = atoi(sector_str);
        print("Reading the sector of the disk...\n");

        char sector_data[512]; 
        read_sector(sector_number, sector_data);
        if (sector_number == 0) {
            print("Reading complete. verifying firm the sector 0...\n");
            unsigned char byte_510 = sector_data[510];
            unsigned char byte_511 = sector_data[511];
            if (byte_510 == 0x55 && byte_511 == 0xAA) {
                print("¡SUCCESS! firm 0x55AA found (Bootloader validate).\n");
            } else {
                print("Fail. firm not found of the bootloader.\n");
            }
        } else {
            print("content of the sector "); print(sector_str); print(":\n");
            sector_data[511] = '\0'; 
            print(sector_data);
            print("\n");
        }
    }
}
void cmd_math() {
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
        } else if (strcmp(arg2, "+") == 0) {
            int result = arg1_num + arg2_num;
            char result_str[10];
            itoa(result, result_str);
            print("the result is: ");print(result_str);print("\n");
        } else if (strcmp(arg2, "-") == 0) {
            int result = arg1_num - arg2_num;
            char result_str[10];
            itoa(result, result_str);
            print("the result is: ");print(result_str);print("\n");
        } else if (strcmp(arg2, "*") == 0) {
            int result = arg1_num * arg2_num;
            char result_str[10];
            itoa(result, result_str);
            print("the result is: ");print(result_str);print("\n");
        }
    }
}
void cmd_write() {
    char* file_name = strtok(0, ' ');
    char* file_ext = strtok(0, ' ');
    char* content = strtok(0, '\0');

    if (file_name == 0 || file_ext == 0 || content == 0) {
        print("Error: missing args\n- use: write [name(8)] [EXT(8)] [text...]\n");
        return;
    }

    char fat_name[12];
    int n_len = strlen(file_name);
    for (int i=0; i<8; i++) fat_name[i] = (i<n_len) ? file_name[i]: ' ';
    int e_len = strlen(file_ext);
    for (int i=0; i<3; i++) fat_name[i+8] = (i<e_len) ? file_ext[i]: ' ';

    fat_name[11] = '\0';
    write_file(fat_name, content);
}
void cmd_draw() { 
    draw_rectangle(10, 10, 50, 50, 40);
}
void cmd_fsinfo() {
    read_fs_info();
}
void cmd_ls() { 
    list_files();
}
void cmd_mkdummy() {
    create_dummy_file();
}
void cmd_mkdir() {
    char* dirname = strtok(0, ' ');
    if (dirname == 0) {
        print("Error of sintax.\n- use: mkdir [name]\n");
        return;
    }

    make_directory(dirname);
}
void cmd_cat() {
    char* file_to_read = strtok(0, ' '); 
    
    if (file_to_read == 0) {
        print("Error: missing args\n- use: cat [filename]\n");
    } else {
        cat_file(file_to_read);
    }
} 
void cmd_cd() {
    char* dirname = strtok(0, ' ');

    if (dirname == 0) {
        print("Error of sintax\n- use: cd [dirname]\n");
        return;
    }
    change_directory(dirname);
}
void cmd_crash() {
    __asm__ volatile("int $0x0E");
}
void cmd_clear() {
    clear_screen();
}
void cmd_beep() {
    print("Emitiendo pitido en el hardware...\n");
    beep();
}
void cmd_sleep() {
    print("killing the cpu\n");
    print("stop getting any interrups\n");
    __asm__ volatile("cli; hlt");
}
void cmd_cpuinfo() {
    get_cpu_info();
}
void cmd_uptime() {
    unsigned int seconds = get_uptime_second();
    char str_seconds[32];
    itoa(seconds, str_seconds);
    print("the system has ");
    print(str_seconds);
    print(" seconds on\n");
}
void cmd_time() {
    print_time();
}
void cmd_alloc() {
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
}
void cmd_lspci() { 
    check_all_pci_buses();
}
void cmd_rm() {
    char* target = strtok(0, ' ');
    if (target == 0) {
        print("Error: missing args.\nUse: rm [name]\n");
        return;
    }
    remove_file(target);
}
void cmd_run() {
    char* target = strtok(0, ' ');
    if (target == 0) {
        print("Use: run [name]\n");
        return;
    }
    execute_program(target);
}
void cmd_mkapp() {
    char app_code[5] = { 0xCD, 0x80, 0xEB, 0xFE, 0x00 };
    write_file("APP     BIN", app_code); 
    print("Program compiled and installed as APP.BIN\n");
}