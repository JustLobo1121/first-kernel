#include "string.h"
static char* last_token = 0;

int strcmp(char s1[], char s2[]) {
    int i=0;

    while (s1[i] == s2[i]) {
        if (s1[i] == '\0') {
            return 0;
        }
        i++;
    }
    return s1[i] - s2[i];
}

int strncmp(char s1[], char s2[], int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

char* strtok(char* str, char delim) {
    if (str != 0) {
        last_token = str;
    }
    if (last_token == 0 || *last_token == '\0') {
        return 0;
    }
    char* token_start = last_token;
    while (*last_token != '\0') {
        if (*last_token == delim) {
            *last_token = '\0';
            last_token++;
            return token_start;
        }
        last_token++;
    }
    return token_start;
}
int atoi(char* str) {
    int res = 0;
    for (int i=0; str[i] != '\0'; i++) {
        res = res * 10 + str[i] - '0';
    }
    return res;
}

void hex_to_string(unsigned int n, char* str) {
    str[0] = '0'; str[1] = 'x';
    char* hex_chars = "0123456789ABCDEF";

    for (int i=7; i>=0; i--) {
        str[i+2] = hex_chars[n & 0x0F];
        n >>= 4;
    }
    str[10] = '\0';
}