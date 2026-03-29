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

void keyboard_handler() {
    unsigned char scancode = port_bytes_in(0x60);

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
