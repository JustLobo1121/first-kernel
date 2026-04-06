#include "ports.h"

unsigned int timer_ticks = 0;
unsigned int current_frecuency = 18;

void init_timer(unsigned int frecuency) {
    current_frecuency = frecuency;
    unsigned int divisor = 1193180 / frecuency;
    port_bytes_out(0x43, 0x36);
    port_bytes_out(0x40, (unsigned char)(divisor & 0xFF));
    port_bytes_out(0x40, (unsigned char)((divisor >> 8) & 0xFF));
}

void timer_handler() {
    timer_ticks++;
    port_bytes_out(0x20, 0x20);
}

unsigned int get_uptime_second() {
    return timer_ticks / current_frecuency;
}