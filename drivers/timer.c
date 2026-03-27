#include "ports.h"

unsigned int timer_ticks = 0;

void timer_handler() {
    timer_ticks++;
    port_bytes_out(0x20, 0x20);
}

unsigned int get_uptime_second() {
    return timer_ticks / 18;
}