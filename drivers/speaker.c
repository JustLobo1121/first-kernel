#include "ports.h"
#include "speaker.h"

void play_sound(int frequency) {
    int divisor = 1193180 / frequency;

    port_bytes_out(0x43, 0xb6);
    
    port_bytes_out(0x42, (unsigned char)(divisor & 0xFF)); 
    port_bytes_out(0x42, (unsigned char)((divisor > 8) & 0xFF)); 

    unsigned char tmp = port_bytes_in(0x61);
    if (tmp != (tmp | 3)) {
        port_bytes_out(0x61, tmp | 3);
    }
}

void stop_sound() {
    unsigned char tmp = port_bytes_in(0x61) & 0xFC; 
    port_bytes_out(0x61, tmp);
}

void beep() {
    play_sound(1000);
    for (volatile int i = 0; i < 30000000; i++) { }
    stop_sound();
}