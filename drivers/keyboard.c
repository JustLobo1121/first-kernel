#include "ports.h"
#include "screen.h"
#include "string.h"
#include "shell.h"
#define NUM_COMMANDS 26

char* os_commands[] = {
    "help", "echo", "color", "read", "math", "write", "draw", "fsinfo", "ls", "mkdummy", "mkdir", "cd", "cat",
    "crash", "clear", "beep", "sleep", "cpuinfo", "uptime", "time", "alloc", "lspci", "rm", "run", "mkapp", "kmsg"
};

extern void print_char(char message);
extern void print(char* message, ...);
extern int strncmp(char s1[], char s2[], int n);
extern void execute_command(char* input);

const char scancode_to_char[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};
const char scancode_to_char_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

int shift_pressed = 0;
int caps_locks = 0;
char key_buffer[256];
int buffer_index = 0;
char last_command[256];
int last_command_len = 0;

void autocomplete_tab() {
    if (buffer_index == 0) return;
    
    key_buffer[buffer_index] = '\0';
    for (int i=0; i<NUM_COMMANDS; i++) {
        if (strncmp(key_buffer, os_commands[i], buffer_index) == 0) {
            int total_len = strlen(os_commands[i]);
            int missing = total_len - buffer_index;
            
            for (int j = 0; j < missing; j++) {
                char letter = os_commands[i][buffer_index];
                key_buffer[buffer_index] = letter;
                buffer_index++;
                
                print_char(letter);
            }
            return;
        }
    }
}

void keyboard_handler() {
    unsigned char scancode = port_bytes_in(0x60);

    if (scancode == 0x0F) {
        autocomplete_tab();
        port_bytes_out(0x20, 0x20);
        return;
    }

    if (scancode == 0x48 && last_command_len > 0) {
        while (buffer_index > 0) {
            print_char ('\b');
            buffer_index--;
        }
        for (int i=0; i<last_command_len; i++) {
            key_buffer[i] = last_command[i];
            print_char(key_buffer[i]);
        }
        buffer_index = last_command_len;
        key_buffer[buffer_index] = '\0';

        port_bytes_out(0x20,0x20);
        return;
    }

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
    } else if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
    } else if (scancode == 0x3A) {
        caps_locks = !caps_locks;
    } else if (scancode <= 57) {
        char letter_normal = scancode_to_char[scancode];
        char letter_shift = scancode_to_char_shift[scancode];
        char final_char = letter_normal;

        int is_letter = (letter_normal >= 'a' && letter_normal <= 'z');
        if (shift_pressed) final_char = letter_shift;

        if (caps_locks && is_letter) {
            if (shift_pressed) final_char = letter_normal;
            else final_char = letter_shift;   
        }
        if (final_char == '\n') {
            print_char('\n');
            key_buffer[buffer_index] = '\0';
            if (buffer_index > 0) {
                for(int i = 0; i <= buffer_index; i++) {
                    last_command[i] = key_buffer[i];
                }
                last_command_len = buffer_index;
            }
            execute_command(key_buffer);
            key_buffer[0] = '\0';
            buffer_index = 0;
            print("OS> ");
        } else if (final_char == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                key_buffer[buffer_index] = '\0';
                print_char('\b');
            }
        } else if (final_char != 0) {
            if (buffer_index < 255) {
                key_buffer[buffer_index] = final_char;
                buffer_index++;
                print_char(final_char);
            }
        }
    }
    port_bytes_out(0x20, 0x20);
}