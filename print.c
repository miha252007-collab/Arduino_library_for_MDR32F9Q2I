#include "print.h"
#include <string.h>
#include <math.h>   

static void write_buf(Print* p, const uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        p->write_byte(p, buf[i]);
    }
}

void print_str(Print* p, const char* str) {
    if (str) write_buf(p, (const uint8_t*)str, strlen(str));
}

void print_char(Print* p, char ch) {
    p->write_byte(p, (uint8_t)ch);
}

void print_uint(Print* p, uint32_t num, uint8_t base) {
    if (base < 2) base = 10;       
    char buffer[32];
    char* ptr = buffer + sizeof(buffer) - 1;
    *ptr = '\0';

    if (num == 0) {
        print_char(p, '0');
        return;
    }
    do {
        uint8_t digit = num % base;
        *(--ptr) = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        num /= base;
    } while (num);
    print_str(p, ptr);
}

void print_int(Print* p, int32_t num, uint8_t base) {
    if (base == DEC && num < 0) {
        print_char(p, '-');
        num = -num;
    }
    print_uint(p, (uint32_t)num, base);
}

void print_hex(Print* p, uint32_t num) {
    print_uint(p, num, HEX);
}

void print_bin(Print* p, uint32_t num) {
    print_uint(p, num, BIN);
}

void print_double(Print* p, double num, uint8_t digits) {
    if (isnan(num)) {
        print_str(p, "nan");
        return;
    }
    if (isinf(num)) {
        print_str(p, "inf");
        return;
    }
    if (num > 4294967040.0 || num < -4294967040.0) {
        print_str(p, "ovf");
        return;
    }

    if (num < 0.0) {
        print_char(p, '-');
        num = -num;
    }

    double rounding = 0.5;
    for (uint8_t i = 0; i < digits; i++)
        rounding /= 10.0;
    num += rounding;

    uint32_t int_part = (uint32_t)num;
    double remainder = num - (double)int_part;
    print_uint(p, int_part, DEC);

    if (digits > 0) {
        print_char(p, '.');
        while (digits--) {
            remainder *= 10.0;
            uint8_t digit = (uint8_t)remainder;
            print_char(p, '0' + digit);
            remainder -= (double)digit;
        }
    }
}

void println(Print* p) {
    print_char(p, '\r');
    print_char(p, '\n');
}

void println_str(Print* p, const char* str) {
    print_str(p, str);
    println(p);
}

void println_char(Print* p, char ch) {
    print_char(p, ch);
    println(p);
}

void println_int(Print* p, int32_t num, uint8_t base) {
    print_int(p, num, base);
    println(p);
}

void println_uint(Print* p, uint32_t num, uint8_t base) {
    print_uint(p, num, base);
    println(p);
}

void println_double(Print* p, double num, uint8_t digits) {
    print_double(p, num, digits);
    println(p);
}

int print_get_error(Print* p) {
    return p->write_error;
}

void print_clear_error(Print* p) {
    p->write_error = 0;
}