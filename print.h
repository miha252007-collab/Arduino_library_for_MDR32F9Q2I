#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>
#include <stddef.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

typedef struct Print Print;
struct Print {
    void (*write_byte)(Print* self, uint8_t byte);

    int write_error;
};

void print_str(Print* p, const char* str);
void print_char(Print* p, char ch);
void print_int(Print* p, int32_t num, uint8_t base);
void print_uint(Print* p, uint32_t num, uint8_t base);
void print_double(Print* p, double num, uint8_t digits);
void print_hex(Print* p, uint32_t num);         
void print_bin(Print* p, uint32_t num);          

void println(Print* p);
void println_str(Print* p, const char* str);
void println_char(Print* p, char ch);
void println_int(Print* p, int32_t num, uint8_t base);
void println_uint(Print* p, uint32_t num, uint8_t base);
void println_double(Print* p, double num, uint8_t digits);

int  print_get_error(Print* p);
void print_clear_error(Print* p);

#endif