#include "ports.h"

extern void itoa(int n, char str[]);
extern void print(char* message, ...);

unsigned char get_rtc_register(int reg) {
    port_bytes_out(0x70, reg);
    return port_bytes_in(0x71);
}

void print_time() {
    unsigned char h = get_rtc_register(0x04);
    unsigned char m = get_rtc_register(0x02);
    unsigned char s = get_rtc_register(0x00);

    h = (h & 0x0F) + ((h / 16) * 10);
    m = (m & 0x0F) + ((m / 16) * 10);
    s = (s & 0x0F) + ((s / 16) * 10);

    char str_h[10], str_m[10], str_s[10];
    itoa(h,str_h); itoa(m,str_m); itoa(s,str_s);
    print("local hour of the hardware: ");
    print(str_h); print(":");
    print(str_m); print(":");
    print(str_s); print("\n");
}