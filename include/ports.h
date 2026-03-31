#ifndef PORTS_H
#define PORTS_H

unsigned char port_bytes_in(unsigned short port);
void port_bytes_out(unsigned short port, unsigned char data);
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned short data);
unsigned int port_dword_in(unsigned int port);
void port_dword_out(unsigned short port, unsigned int data);

#endif