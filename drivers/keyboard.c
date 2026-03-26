#include "ports.h"

extern void print_char(char message);
extern void print(char* message, ...);
extern void execute_command(char* input);

const char scancode_to_char[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

char key_buffer[256];
int buffer_index = 0;

void keyboard_handler() {
    unsigned char scancode = port_bytes_in(0x60);

    if (scancode <= 57) {
        char letter = scancode_to_char[scancode];

        if (letter == '\n') {
            print_char('\n');
            key_buffer[buffer_index] = '\0';
            execute_command(key_buffer);
            key_buffer[0] = '\0';
            buffer_index = 0;
            print("OS> ");
        } else if (letter == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                key_buffer[buffer_index] = '\0';
                print_char('\b');
            }
        } else {
            if (buffer_index < 255) {
                key_buffer[buffer_index] = letter;
                buffer_index++;
                print_char(letter);
            }
        }
    }
    port_bytes_out(0x20, 0x20);
}
