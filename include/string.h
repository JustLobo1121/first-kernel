#ifndef STRING_H
#define STRING_H

int strcmp(char s1[], char s2[]);
int strncmp(char s1[], char s2[], int n);
char* strtok(char* str, char delim);
int strlen(char s[]);
int atoi(char* str);
void reverse(char str[], int length);
void itoa(int n, char str[]);
void hex_to_string(unsigned int n, char* str);

#endif