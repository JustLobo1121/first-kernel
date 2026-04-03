#include "screen.h"

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