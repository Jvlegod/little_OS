#ifndef LOG_H
#define LOG_H
#include "comm/types.h"
#include "comm/cpu_instr.h"

#define PORT_COM1 0x3f8 // COM1

void log_init();
bool is_transmit_empty();
void write_serial(char a);
void write_serial_end();

#endif // !LOG_H