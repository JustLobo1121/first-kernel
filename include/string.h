#ifndef STRING_H
#define STRING_H

int strcmp(char s1[], char s2[]);
int strncmp(char s1[], char s2[], int n);
char* strtok(char* str, char delim);
int atoi(char* str);
void hex_to_string(unsigned int n, char* str);

#endif